/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLIOImageDevice.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioVideoDevice.h"

#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include "vtkMatrix4x4.h"

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioVideoDeviceCreator::Create(std::string device_name)
{
 igtlioVideoDevicePointer retval = igtlioVideoDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string igtlioVideoDeviceCreator::GetDeviceType() const
{
  return igtlioVideoConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioVideoDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioVideoDevice);
//---------------------------------------------------------------------------
igtlioVideoDevice::igtlioVideoDevice()
{
  VideoStreamDecoderH264 = NULL;
  VideoStreamEncoderH264 = NULL;
  VideoStreamDecoderVPX  = NULL;
  VideoStreamEncoderVPX  = NULL;
  VideoStreamDecoderX265 = NULL;
  VideoStreamEncoderX265 = NULL;
  VideoStreamDecoderAV1 = NULL;
  VideoStreamEncoderAV1 = NULL;
#if defined(OpenIGTLink_USE_H264)
  VideoStreamDecoderH264 = new H264Decoder();
  VideoStreamEncoderH264 = new H264Encoder();
  VideoStreamEncoderH264->InitializeEncoder();
  VideoStreamEncoderH264->SetLosslessLink(true);
#endif
#if defined(OpenIGTLink_USE_VP9)
  VideoStreamDecoderVPX = new VP9Decoder();
  VideoStreamEncoderVPX = new VP9Encoder();
  VideoStreamEncoderVPX->SetPicWidthAndHeight(256,256);
  VideoStreamEncoderVPX->InitializeEncoder();
  VideoStreamEncoderVPX->SetLosslessLink(true);
#endif
#if defined(OpenIGTLink_USE_AV1)
  VideoStreamDecoderAV1 = new igtlAV1Decoder();
  VideoStreamEncoderAV1 = new igtlAV1Encoder();
  VideoStreamEncoderAV1->SetPicWidthAndHeight(256,256);
  VideoStreamEncoderAV1->SetLosslessLink(true);
  VideoStreamEncoderAV1->InitializeEncoder();
#endif
#if defined(OpenIGTLink_USE_OpenHEVC)
  VideoStreamDecoderX265 = new H265Decoder();
#endif
#if defined(OpenIGTLink_USE_X265)
  VideoStreamEncoderX265 = new H265Encoder();
  VideoStreamEncoderX265->SetLosslessLink(true);
  VideoStreamEncoderX265->InitializeEncoder();
#endif

  DecodersMap.clear();
  DecodersMap.insert(std::pair<std::string, GenericDecoder*>(IGTL_VIDEO_CODEC_NAME_H264,VideoStreamDecoderH264));
  DecodersMap.insert(std::pair<std::string, GenericDecoder*>(IGTL_VIDEO_CODEC_NAME_VP9, VideoStreamDecoderVPX));
  DecodersMap.insert(std::pair<std::string, GenericDecoder*>(IGTL_VIDEO_CODEC_NAME_OPENHEVC,VideoStreamDecoderX265));
  DecodersMap.insert(std::pair<std::string, GenericDecoder*>(IGTL_VIDEO_CODEC_NAME_AV1,
      VideoStreamDecoderAV1));
  DecodedPic = new SourcePicture();
  this->CurrentCodecType = IGTL_VIDEO_CODEC_NAME_VP9;
}

//---------------------------------------------------------------------------
igtlioVideoDevice::~igtlioVideoDevice()
{
  DecodersMap.clear();
}

//---------------------------------------------------------------------------
unsigned int igtlioVideoDevice::GetDeviceContentModifiedEvent() const
{
  return VideoModifiedEvent;
}


//---------------------------------------------------------------------------
std::string igtlioVideoDevice::GetDeviceType() const
{
  return igtlioVideoConverter::GetIGTLTypeName();
}

void igtlioVideoDevice::SetContent(igtlioVideoConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(VideoModifiedEvent, this);
}

igtlioVideoConverter::ContentData igtlioVideoDevice::GetContent()
{
  return Content;
}

igtl::VideoMessage::Pointer  igtlioVideoDevice::GetCompressedIGTLMessage()
{
  igtl::VideoMessage::Pointer videoMessage = igtl::VideoMessage::New();
  videoMessage->InitPack();
  videoMessage->Copy(this->Content.videoMessage);
  return videoMessage;
}

igtl::VideoMessage::Pointer  igtlioVideoDevice::GetKeyFrameMessage()
{
  igtl::VideoMessage::Pointer videoMessage = igtl::VideoMessage::New();
  videoMessage->InitPack();
  videoMessage->Copy(this->Content.keyFrameMessage);
  return videoMessage;
}

//---------------------------------------------------------------------------
int igtlioVideoDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->Copy(buffer);
  if(strcmp(headerMsg->GetDeviceName(), this->GetDeviceName().c_str())==0)
    {
    // Copy the current received video message

    int returnValue = 0;
    //To Do, we need to unpack the buffer to know the codec type, which is done in the converter
    // So the user need to set the correct CurrentCodecType before hand.
    returnValue = igtlioVideoConverter::fromIGTL(buffer, &HeaderData, &Content, this->DecodersMap, checkCRC, this->metaInfo);

    if (returnValue)
     {
     this->SetCurrentCodecType(std::string(Content.codecName));
     this->Modified();
     this->InvokeEvent(VideoModifiedEvent, this);
     return 1;
     }
  }
 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioVideoDevice::GetIGTLMessage()
{
 if (!Content.image)
  {
  vtkWarningMacro("Video is NULL, message not generated.")
  return 0;
  }
  int imageSizePixels[3] = { 0 };
  Content.image->GetDimensions(imageSizePixels);
  float bitRatePercent = 0.05;
  int frameRate = 20;
  int iReturn = 0;
  if(Content.videoMessage.IsNull())
    Content.videoMessage = igtl::VideoMessage::New();
#if defined(OpenIGTLink_USE_H264)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_H264) == 0)
    {
    VideoStreamEncoderH264->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    //newEncoder->SetKeyFrameDistance(25);
    VideoStreamEncoderH264->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
    Content.videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_H264);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, VideoStreamEncoderH264, &this->metaInfo);
    }
#endif
#if defined(OpenIGTLink_USE_VP9)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_VP9) == 0)
    {
    VideoStreamEncoderVPX->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    //newEncoder->SetKeyFrameDistance(25);
    VideoStreamEncoderVPX->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
    Content.videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_VP9);
    iReturn = igtlioVideoConverter::toIGTL(HeaderData, Content, VideoStreamEncoderVPX, this->metaInfo);
    }
#endif
#if defined(OpenIGTLink_USE_X265)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_X265) == 0)
    {
    VideoStreamEncoderX265->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    int bitRateFactor = 7;
    VideoStreamEncoderX265->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent)*bitRateFactor);
    Content.videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_X265);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, VideoStreamEncoderX265, &this->metaInfo);
    }
#endif
  int frameType = Content.videoMessage->GetFrameType();
  Content.keyFrameUpdated = (frameType == igtl::FrameTypeKey);
 if (!iReturn)
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(Content.videoMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioVideoDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<igtlioDevice::MESSAGE_PREFIX> igtlioVideoDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

//---------------------------------------------------------------------------
void igtlioVideoDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  igtlioDevice::PrintSelf(os, indent);

  os << indent << "Video:\t" <<"\n";
  Content.image->PrintSelf(os, indent.GetNextIndent());
}
