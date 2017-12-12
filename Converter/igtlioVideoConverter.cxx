/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
 Version:   $Revision: 15621 $
 
 ==========================================================================*/

#include "igtlioVideoConverter.h"

#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkVersion.h>

namespace igtlio
{
  //---------------------------------------------------------------------------
  int VideoConverter::fromIGTL(igtl::MessageBase::Pointer source,
                               HeaderData* header,
                               ContentData* dest,
                               std::map<std::string,GenericDecoder*> decoders,
                               bool checkCRC, igtl::MessageBase::MetaDataMap* metaInfo)
  {
    // Create a message buffer to receive image data
    igtl::VideoMessage::Pointer videoMsg;
    videoMsg = igtl::VideoMessage::New();
    videoMsg->Copy(source); // !! TODO: copy makes performance issue.
    
    // Deserialize the data
    // If CheckCRC==0, CRC check is skipped.
    int c = videoMsg->Unpack(checkCRC);
    
    if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
      // TODO: error handling
      return 0;
    }
    GenericDecoder* decoder = NULL;
#if defined(USE_H264)
  if(videoMsg->GetCodecType().compare(CodecNameForH264)==0)
    {
    decoder = decoders.find(CodecNameForH264)->second;
    }
#endif
#if defined(USE_VP9)
  if(videoMsg->GetCodecType().compare(CodecNameForVPX)==0)
    {
    decoder = decoders.find(CodecNameForVPX)->second;
    }
#endif
#if defined(USE_OpenHEVC)
    if(videoMsg->GetCodecType().compare(CodecNameForX265)==0)
    {
    decoder = decoders.find(CodecNameForX265)->second;
    }
#endif
    
    // get header
    if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(videoMsg), header, metaInfo))
      return 0;
    
    // get Video
    if (VideoConverter::IGTLToVTKImageData(videoMsg, dest, decoder) == 0)
      return 0;
    
    return 1;
  }
  
  
  //---------------------------------------------------------------------------
  int VideoConverter::IGTLToVTKImageData(igtl::VideoMessage::Pointer videoMsg, ContentData *dest,GenericDecoder * videoStreamDecoder)
  {
    if (!dest->image)
      dest->image = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> imageData = dest->image;
    if(videoStreamDecoder)
    {
      int32_t Width = videoMsg->GetWidth();
      int32_t Height = videoMsg->GetHeight();
      if (videoMsg->GetWidth() != imageData->GetDimensions()[0] ||
          videoMsg->GetHeight() != imageData->GetDimensions()[1])
      {
        imageData->SetDimensions(Width , Height, 1);
        imageData->SetExtent(0, Width-1, 0, Height-1, 0, 0 );
        imageData->SetOrigin(0, 0, 0);
        imageData->AllocateScalars(VTK_UNSIGNED_CHAR,3);
      }
      SourcePicture* pDecodedPic = new SourcePicture();
      pDecodedPic->data[0] = new igtl_uint8[Width * Height*3/2];
      memset(pDecodedPic->data[0], 0, Width * Height * 3 / 2);
      if(!videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMsg, pDecodedPic))
      {
        pDecodedPic->~SourcePicture();
        return 0;
      }
      igtl_uint16 frameType = videoMsg->GetFrameType();
      bool isGrayImage = false;
      if(frameType > 0x00FF)//Using first byte of video frame type to indicate gray or color video. It might be better to change the video stream protocol to add additional field for indicating Gray or color image.
      {
        isGrayImage =  true;
        frameType = frameType >> 8;
      }
      else
      {
        isGrayImage =  false;
      }
      if (isGrayImage)
      {
        videoStreamDecoder->ConvertYUVToGrayImage(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(), Height, Width);
      }
      else
      {
        videoStreamDecoder->ConvertYUVToRGB(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(),Height, Width);
      }
      imageData->Modified();
      delete pDecodedPic;
    }
    return 1;
  }
  
  //---------------------------------------------------------------------------
  int VideoConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::VideoMessage::Pointer* dest, GenericEncoder* encoder, igtl::MessageBase::MetaDataMap* metaInfo)
  {
    if (dest->IsNull())
      *dest = igtl::VideoMessage::New();
    (*dest)->InitPack();
    igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(*dest);
    
    HeadertoIGTL(header, &basemsg, metaInfo);
    
    igtl::VideoMessage::Pointer videoMsg = *dest;
    if (metaInfo!=NULL)
      videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    
    vtkImageData* frameImage = source.image;
    int   scalarType = frameImage->GetScalarType();       // scalar type, currently only unsigned char is supported
    int   ncomp = frameImage->GetNumberOfScalarComponents();
    if (ncomp != 3 && (scalarType != videoMsg->TYPE_INT8 || scalarType != videoMsg->TYPE_UINT8) )
    {
     std::cerr<<"Invalid image data format!";
      return 0;
    }
    
    if (encoder == NULL)
    {
      std::cerr<<"Failed to pack video message - input video message encoder is NULL";
      return 0;
    }
    
    if (videoMsg.IsNull())
    {
      std::cerr<<"Failed to pack video message - input video message is NULL";
      return 0;
    }
    
    if (!source.image->GetScalarPointer())
    {
      std::cerr<<"Unable to send video message - image data is NOT valid!";
      return 0;
    }
    
    int imageSizePixels[3] = { 0 };
    
    frameImage->GetDimensions(imageSizePixels);
    
    unsigned char* YUV420ImagePointer = new unsigned char[imageSizePixels[0] * imageSizePixels[1]*3/2];
    
    encoder->ConvertRGBToYUV((igtlUint8*)frameImage->GetScalarPointer(), YUV420ImagePointer, imageSizePixels[0], imageSizePixels[1]);
    int iSourceWidth = imageSizePixels[0];
    int iSourceHeight = imageSizePixels[1];
    SourcePicture* pSrcPic = new SourcePicture();
    pSrcPic->colorFormat = FormatI420; // currently only format 420 is supported.
    pSrcPic->timeStamp = 0;
    pSrcPic->picWidth  = imageSizePixels[0];
    pSrcPic->picHeight = imageSizePixels[1];
    if (encoder->GetPicHeight() != iSourceHeight
        || encoder->GetPicWidth() != iSourceWidth)
    {
      encoder->SetPicWidthAndHeight(iSourceWidth,iSourceHeight);
      encoder->InitializeEncoder();
    }
    pSrcPic->data[0] = YUV420ImagePointer;
    pSrcPic->data[1] = pSrcPic->data[0] + (iSourceWidth * iSourceHeight);
    pSrcPic->data[2] = pSrcPic->data[1] + (iSourceWidth * iSourceHeight >> 2);
    bool isGrayImage = false;
    
    static int frameIndex = 0;
    frameIndex++;
    videoMsg->SetMessageID(frameIndex);
    int iEncFrames = encoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMsg.GetPointer(), isGrayImage);
    delete[] YUV420ImagePointer;
    YUV420ImagePointer = NULL;
    if (iEncFrames != 0) //OpenIGTLink library returns 0 when successful.
      return 0;
    return 1;
  }
  //---------------------------------------------------------------------------
  int VideoConverter::IGTLToVTKScalarType(int igtlType)
  {
    switch (igtlType)
    {
      case igtl::VideoMessage::TYPE_INT8: return VTK_CHAR;
      case igtl::VideoMessage::TYPE_UINT8: return VTK_UNSIGNED_CHAR;
      case igtl::VideoMessage::TYPE_INT16: return VTK_SHORT;
      case igtl::VideoMessage::TYPE_UINT16: return VTK_UNSIGNED_SHORT;
      case igtl::VideoMessage::TYPE_INT32: return VTK_UNSIGNED_LONG;
      case igtl::VideoMessage::TYPE_UINT32: return VTK_UNSIGNED_LONG;
      default:
        std::cerr << "Invalid IGTL scalar Type: "<<igtlType << std::endl;
        return VTK_VOID;
    }
  }
  
} //namespace igtlio
