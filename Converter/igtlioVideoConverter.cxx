/*==========================================================================

 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
 Version:   $Revision: 15621 $

 ==========================================================================*/

#include "igtlioVideoConverter.h"

#include "igtlioConverterUtilities.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVersion.h>

//---------------------------------------------------------------------------
int igtlioVideoConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             std::map<std::string,GenericDecoder*> decoders,
                             bool checkCRC,
                             igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Create a message buffer to receive image data
  if (dest->videoMessage.IsNull())
    dest->videoMessage = igtl::VideoMessage::New();
  dest->videoMessage->Copy(source); // We would like to save the untouched data before the Unpack()
  igtl::VideoMessage::Pointer videoMessage = igtl::VideoMessage::New();
  videoMessage->InitPack();
  videoMessage->Copy(source);
  // Deserialize the data
  // If CheckCRC==0, CRC check is skipped.
  int c = videoMessage->Unpack(checkCRC);

  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }
  igtl_uint16 frameType = videoMessage->GetFrameType();
  if (frameType>0X00FF)
    {
    frameType= frameType>>8;
    }
  dest->frameType = (VideoFrameType)frameType;
  strncpy(dest->codecName, "    ", IGTL_VIDEO_CODEC_NAME_SIZE);
  GenericDecoder* decoder = NULL;

  if (videoMessage->GetCodecType().compare(IGTL_VIDEO_CODEC_NAME_I420) == 0)
  {
    decoder = decoders.find(IGTL_VIDEO_CODEC_NAME_I420)->second;
    strncpy(dest->codecName, IGTL_VIDEO_CODEC_NAME_I420, IGTL_VIDEO_CODEC_NAME_SIZE);
  }
#if defined(OpenIGTLink_USE_H264)
 if(videoMessage->GetCodecType().compare(IGTL_VIDEO_CODEC_NAME_H264)==0)
   {
   decoder = decoders.find(IGTL_VIDEO_CODEC_NAME_H264)->second;
   strncpy(dest->codecName, IGTL_VIDEO_CODEC_NAME_H264, IGTL_VIDEO_CODEC_NAME_SIZE);
   }
#endif
#if defined(OpenIGTLink_USE_VP9)
  if(videoMessage->GetCodecType().compare(IGTL_VIDEO_CODEC_NAME_VP9)==0)
    {
    decoder = decoders.find(IGTL_VIDEO_CODEC_NAME_VP9)->second;
    strncpy(dest->codecName, IGTL_VIDEO_CODEC_NAME_VP9, IGTL_VIDEO_CODEC_NAME_SIZE);
    }
#endif
#if defined(OpenIGTLink_USE_AV1)
  if(videoMessage->GetCodecType().compare(IGTL_VIDEO_CODEC_NAME_AV1)==0)
    {
    decoder = decoders.find(IGTL_VIDEO_CODEC_NAME_AV1)->second;
    strncpy(dest->codecName, IGTL_VIDEO_CODEC_NAME_AV1, IGTL_VIDEO_CODEC_NAME_SIZE);
    }
#endif
#if defined(OpenIGTLink_USE_OpenHEVC)
  // videoMessage was encoded using X265 at the server side, so the codec type was set to "IGTL_VIDEO_CODEC_NAME_X265" by the server.
  if(videoMessage->GetCodecType().compare(IGTL_VIDEO_CODEC_NAME_X265)==0)
    {
    decoder = decoders.find(IGTL_VIDEO_CODEC_NAME_OPENHEVC)->second;
    strncpy(dest->codecName, IGTL_VIDEO_CODEC_NAME_X265, IGTL_VIDEO_CODEC_NAME_SIZE);
    }
#endif
  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(videoMessage), header, outMetaInfo))
    return 0;

  if (!dest->transform)
    dest->transform = vtkSmartPointer<vtkMatrix4x4>::New();

  // get transform
  if (igtlioVideoConverter::IGTLFrameToVTKTransform(videoMessage, dest->transform) == 0)
    return 0;

  // get Video
  if (igtlioVideoConverter::IGTLToVTKImageData(dest, videoMessage, decoder) == 0)
    return 0;

  if (!dest->frameData)
  {
    dest->frameData = vtkSmartPointer<vtkUnsignedCharArray>::New();
  }

  vtkSmartPointer<vtkUnsignedCharArray> frameData = dest->frameData;
  frameData->Allocate(videoMessage->GetBitStreamSize());
  memcpy(frameData->GetPointer(0), videoMessage->GetPackFragmentPointer(2), videoMessage->GetBitStreamSize());

  return 1;
}

//---------------------------------------------------------------------------
int igtlioVideoConverter::IGTLToVTKImageData(ContentData *dest, igtl::VideoMessage::Pointer videoMessage, GenericDecoder *videoStreamDecoder)
{
  if(videoStreamDecoder == NULL)
    {
    std::cerr<<"Failed to decode video message - input video message decoder is NULL";
    }
  if (!dest->image)
    dest->image = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageData> imageData = dest->image;

  int32_t Width = videoMessage->GetWidth();
  int32_t Height = videoMessage->GetHeight();
  if (videoMessage->GetWidth() != imageData->GetDimensions()[0] ||
      videoMessage->GetHeight() != imageData->GetDimensions()[1])
    {
    imageData->SetDimensions(Width , Height, 1);
    imageData->SetExtent(0, Width-1, 0, Height-1, 0, 0 );
    imageData->SetOrigin(0, 0, 0);
    imageData->AllocateScalars(VTK_UNSIGNED_CHAR,3);
    }
  SourcePicture* pDecodedPic = new SourcePicture();
  pDecodedPic->data[0] = new igtl_uint8[Width * Height*3/2];
  memset(pDecodedPic->data[0], 0, Width * Height * 3 / 2);
  if(videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessage, pDecodedPic) == -1)
    {
    pDecodedPic->~SourcePicture();
    return 0;
    }
  igtl_uint16 frameType = videoMessage->GetFrameType();
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
  dest->keyFrameUpdated = false;
  if (frameType == FrameTypeKey)
    {
    if (dest->keyFrameMessage.IsNull())
      dest->keyFrameMessage = igtl::VideoMessage::New();
    dest->keyFrameMessage->Copy(dest->videoMessage); // We would like to track the latest key frame message
    dest->keyFrameUpdated = true;
    }
  if (isGrayImage)
    {
    GenericDecoder::ConvertYUVToGrayImage(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(), Height, Width);
    }
  else
    {
    GenericDecoder::ConvertYUVToRGB(pDecodedPic->data[0], (uint8_t*)imageData->GetScalarPointer(),Height, Width);
    }
  imageData->Modified();
  if (pDecodedPic->data[0]!=NULL)
    delete [] pDecodedPic->data[0];
  delete pDecodedPic;
  return 1;
}

//---------------------------------------------------------------------------
int igtlioVideoConverter::toIGTL(const HeaderData& header, const ContentData& source, GenericEncoder* encoder, igtl::MessageBase::MetaDataMap metaInfo)
{
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(source.videoMessage);

  HeadertoIGTL(header, &basemsg, metaInfo);

  igtl::VideoMessage::Pointer videoMsg = source.videoMessage;

  if (!metaInfo.empty())
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

  double *spacing;       // spacing (mm/pixel)
  spacing = frameImage->GetSpacing();
  scalarType = frameImage->GetScalarType();
  ncomp = frameImage->GetNumberOfScalarComponents();
  
  igtl::Matrix4x4 matrix; // Image origin and orientation matrix
  igtlioConverterUtilities::VTKTransformToIGTLTransform(source.transform, frameImage->GetDimensions(), spacing, matrix);

  // Check endianness of the machine
  int endian = igtl::VideoMessage::ENDIAN_BIG;
  if (igtl_is_little_endian())
    {
    endian = igtl::VideoMessage::ENDIAN_LITTLE;
    }

  videoMsg->SetMatrix(matrix);
  videoMsg->SetSpacing((float)spacing[0], (float)spacing[1], (float)spacing[2]);
  videoMsg->SetEndian(endian);

  int imageSizePixels[3] = { 0 };

  frameImage->GetDimensions(imageSizePixels);

  unsigned char* YUV420ImagePointer = new unsigned char[imageSizePixels[0] * imageSizePixels[1]*3/2];

  GenericEncoder::ConvertRGBToYUV((igtlUint8*)frameImage->GetScalarPointer(), YUV420ImagePointer, imageSizePixels[0], imageSizePixels[1]);
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
int igtlioVideoConverter::IGTLToVTKScalarType(int igtlType)
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

//---------------------------------------------------------------------------
int igtlioVideoConverter::IGTLFrameToVTKTransform(igtl::VideoMessage::Pointer videoMsg, vtkSmartPointer<vtkMatrix4x4> ijk2ras)
{
  igtl::Matrix4x4 matrix;
  videoMsg->GetMatrix(matrix);

  int imageSize[3];
  imageSize[0] = videoMsg->GetWidth();
  imageSize[1] = videoMsg->GetHeight();
  imageSize[2] = videoMsg->GetAdditionalZDimension();

  float imageSpacing[3];
  videoMsg->GetSpacing(imageSpacing);
  igtlioConverterUtilities::IGTLTransformToVTKTransform(imageSize, imageSpacing, matrix, ijk2ras);

  return 1;
}