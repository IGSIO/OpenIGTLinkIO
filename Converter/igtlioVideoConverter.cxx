/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
 Version:   $Revision: 15621 $
 
 ==========================================================================*/

#include "igtlioVideoConverter.h"

#include <igtl_util.h>
#include <igtlVideoMessage.h>

#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkVersion.h>

namespace igtlio
{
  
  VideoConverter::VideoConverter()
  {
    for (int i = 0; i< VideoThreadMaxNumber; i++)
    {
#if OpenIGTLink_LINK_VP9
      VideoStreamDecoderVPX[i] = new VP9Decoder();
#endif
#if OpenIGTLink_LINK_X265
      VideoStreamDecoderX265[i] = new H265Decoder();
#endif
#if OpenIGTLink_LINK_H264
      VideoStreamDecoderH264[i] = new H264Decoder();
#endif
    }
    pDecodedPic = new SourcePicture();
    this->currentCodecType = useVP9;
    igtlFrameTime = igtl::TimeStamp::New();
  }
  
  //---------------------------------------------------------------------------
  int VideoConverter::fromIGTL(igtl::MessageBase::Pointer source,
                               HeaderData* header,
                               ContentData* dest,
                               bool checkCRC)
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
    
    // get header
    if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(videoMsg), header))
      return 0;
    
    // get Video
    if (this->IGTLToVTKImageData(videoMsg, dest) == 0)
      return 0;
    
    return 1;
  }
  
  
  //---------------------------------------------------------------------------
  int VideoConverter::IGTLToVTKImageData(igtl::VideoMessage::Pointer videoMsg, ContentData *dest)
  {
    if (!dest->image)
      dest->image = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> imageData = dest->image;
    int c = videoMsg->Unpack();
    
    if (c == 0) // if CRC check fails
    {
      // TODO: error handling
      return 0;
    }
    std::string deviceName(videoMsg->GetDeviceName()); // buffer has the header information, the videoMsg has not device name information.
    int currentDecoderIndex = -1;
    GenericDecoder * VideoStreamDecoder = NULL;
    for (int i = 0; i < VideoThreadMaxNumber; i++)
    {
      std::string decoderName = "";
      if (videoMsg->GetCodecType().compare(CodecNameForH264) == 0 && OpenIGTLink_LINK_H264)
      {
        decoderName = VideoStreamDecoderH264[i]->GetDeviceName();
        if (deviceName.compare(decoderName) == 0)
        {
          currentDecoderIndex = i;
          VideoStreamDecoder = VideoStreamDecoderH264[currentDecoderIndex];
          break;
        }
      }
      else if(videoMsg->GetCodecType().compare(CodecNameForVPX) == 0 && OpenIGTLink_LINK_VP9)
      {
        decoderName = VideoStreamDecoderVPX[i]->GetDeviceName();
        if (deviceName.compare(decoderName) == 0)
        {
          currentDecoderIndex = i;
          VideoStreamDecoder = VideoStreamDecoderVPX[currentDecoderIndex];
          break;
        }
      }
      else if(videoMsg->GetCodecType().compare(CodecNameForX265) == 0 && OpenIGTLink_LINK_X265)
      {
        decoderName = VideoStreamDecoderX265[i]->GetDeviceName();
        if (deviceName.compare(decoderName) == 0)
        {
          currentDecoderIndex = i;
          VideoStreamDecoder = VideoStreamDecoderX265[currentDecoderIndex];
          break;
        }
      }
    }
    if (currentDecoderIndex<0)
    {
      for (int i = 0; i < VideoThreadMaxNumber; i++)
      {
        std::string decoderName = "";
        if (videoMsg->GetCodecType().compare(CodecNameForH264) == 0 && OpenIGTLink_LINK_H264)
        {
          if (VideoStreamDecoderH264[i]->GetDeviceName().compare("") == 0)
          {
            currentDecoderIndex = i;
            VideoStreamDecoderH264[currentDecoderIndex]->GetDeviceName() = deviceName;
            VideoStreamDecoder = VideoStreamDecoderH264[currentDecoderIndex];
            break;
          }
        }
        else if(videoMsg->GetCodecType().compare(CodecNameForVPX) == 0 && OpenIGTLink_LINK_VP9)
        {
          if (VideoStreamDecoderVPX[i]->GetDeviceName().compare("") == 0)
          {
            currentDecoderIndex = i;
            VideoStreamDecoderVPX[currentDecoderIndex]->GetDeviceName() = deviceName;
            VideoStreamDecoder = VideoStreamDecoderVPX[currentDecoderIndex];
            break;
          }
        }
        else if(videoMsg->GetCodecType().compare(CodecNameForX265) == 0 && OpenIGTLink_LINK_X265)
        {
          if (VideoStreamDecoderX265[i]->GetDeviceName().compare("") == 0)
          {
            currentDecoderIndex = i;
            VideoStreamDecoderX265[currentDecoderIndex]->GetDeviceName() = deviceName;
            VideoStreamDecoder = VideoStreamDecoderX265[currentDecoderIndex];
            break;
          }
        }
      }
    }
    if(currentDecoderIndex>=0 && VideoStreamDecoder)
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
        delete pDecodedPic;
        pDecodedPic = new SourcePicture();
        pDecodedPic->data[0] = new igtl_uint8[Width * Height*3/2];
        memset(pDecodedPic->data[0], 0, Width * Height * 3 / 2);
      }
      if(!VideoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMsg, pDecodedPic))
      {
        pDecodedPic->~SourcePicture();
        return 0;
      }
      igtl_uint16 frameType = videoMsg->GetFrameType();
      bool isGrayImage = false;
      if(frameType > 0x00FF)
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
        VideoStreamDecoder->ConvertYUVToGrayImage(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(), Height, Width);
      }
      else
      {
        VideoStreamDecoder->ConvertYUVToRGB(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(),Height, Width);
      }
      imageData->Modified();
    }
    return 1;
  }
  
  //---------------------------------------------------------------------------
  int VideoConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::VideoMessage::Pointer* dest)
  {
    igtl::VideoMessage::Pointer videoMsg = *dest;
    vtkImageData* frameImage = source.image;
    int   scalarType = frameImage->GetScalarType();       // scalar type, currently only unsigned char is supported
    int   ncomp = frameImage->GetNumberOfScalarComponents();
    if (ncomp != 3 && (scalarType != videoMsg->TYPE_INT8 || scalarType != videoMsg->TYPE_UINT8) )
    {
     std::cerr<<"Invalid image data format!";
      return 0;
    }
    
    if (videoStreamEncoderMap.find(header.deviceName) == videoStreamEncoderMap.end())
    {
      int imageSizePixels[3] = { 0 };
      frameImage->GetDimensions(imageSizePixels);
      float bitRatePercent = 0.05;
      int frameRate = 20;
#ifdef OpenIGTLink_LINK_VP9
      if(this->useVP9)
      {
        VP9Encoder* newEncoder = new VP9Encoder();
        newEncoder->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
        //newEncoder->SetKeyFrameDistance(25);
        newEncoder->SetLosslessLink(false);
        newEncoder->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
        newEncoder->InitializeEncoder();
        newEncoder->SetSpeed(8);
        videoStreamEncoderMap[std::string(header.deviceName)] = newEncoder;
      }
#endif
#ifdef OpenIGTLink_LINK_H265
      if(this->useH265)
      {
        H265Encoder newEncoder = new H265Encoder();
        newEncoder->SetPicWidthAndHeight(trackedFrame.GetFrameSize()[0], trackedFrame.GetFrameSize()[1]);
        int bitRateFactor = 7;
        newEncoder->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent)*bitRateFactor);
        newEncoder->InitializeEncoder();
        newEncoder->SetSpeed(9);
        videoStreamEncoderMap[std::string(header.deviceName)] = newEncoder;
      }
#endif
    }
    GenericEncoder* videoStreamEncoder =videoStreamEncoderMap[header.deviceName];
    if (videoStreamEncoder == NULL)
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
    
    int imageSizePixels[3] = { 0 };  double imageSpacingMm[3] = { 0 };
    
    frameImage->GetDimensions(imageSizePixels);
    frameImage->GetSpacing(imageSpacingMm);
    
    float spacingFloat[3] = { 0 };
    for (int i = 0; i < 3; ++i)
    {
      spacingFloat[i] = (float)imageSpacingMm[i];
    }
    unsigned char* YUV420ImagePointer = new unsigned char[imageSizePixels[0] * imageSizePixels[1]*3/2];
    
    videoStreamEncoder->ConvertRGBToYUV((igtlUint8*)frameImage->GetScalarPointer(), YUV420ImagePointer, imageSizePixels[0], imageSizePixels[1]);
    int iSourceWidth = imageSizePixels[0];
    int iSourceHeight = imageSizePixels[1];
    SourcePicture* pSrcPic = new SourcePicture();
    pSrcPic->colorFormat = FormatI420; // currently only format 420 is supported.
    pSrcPic->timeStamp = 0;
    pSrcPic->picWidth  = imageSizePixels[0];
    pSrcPic->picHeight = imageSizePixels[1];
    if (videoStreamEncoder->GetPicHeight() != iSourceHeight
        || videoStreamEncoder->GetPicWidth() != iSourceWidth)
    {
      videoStreamEncoder->SetPicWidthAndHeight(iSourceWidth,iSourceHeight);
      videoStreamEncoder->InitializeEncoder();
    }
    pSrcPic->data[0] = YUV420ImagePointer;
    pSrcPic->data[1] = pSrcPic->data[0] + (iSourceWidth * iSourceHeight);
    pSrcPic->data[2] = pSrcPic->data[1] + (iSourceWidth * iSourceHeight >> 2);
    bool isGrayImage = true;
    
    static int frameIndex = 0;
    frameIndex++;
    videoMsg->SetMessageID(frameIndex);
    igtlFrameTime->SetTime(header.timestamp);
    videoMsg->SetTimeStamp(igtlFrameTime);
    int iEncFrames = videoStreamEncoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMsg.GetPointer(), isGrayImage);
    delete[] YUV420ImagePointer;
    YUV420ImagePointer = NULL;
    if (iEncFrames == 0)
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
