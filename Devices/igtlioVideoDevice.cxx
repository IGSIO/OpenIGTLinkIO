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

namespace igtlio
{

//---------------------------------------------------------------------------
DevicePointer VideoDeviceCreator::Create(std::string device_name)
{
 VideoDevicePointer retval = VideoDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string VideoDeviceCreator::GetDeviceType() const
{
  return VideoConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(VideoDeviceCreator);


//---------------------------------------------------------------------------
vtkStandardNewMacro(VideoDevice);
//---------------------------------------------------------------------------
VideoDevice::VideoDevice()
{
  this->OutVideoMessage = igtl::VideoMessage::New();
  this->OutVideoMessage->SetHeaderVersion(OpenIGTLink_HEADER_VERSION);
  VideoStreamDecoderH264 = NULL;
  VideoStreamEncoderH264 = NULL;
  VideoStreamDecoderVPX  = NULL;
  VideoStreamEncoderVPX  = NULL;
  VideoStreamDecoderX265 = NULL;
  VideoStreamEncoderX265 = NULL;
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
  DecodedPic = new SourcePicture();
  this->CurrentCodecType = IGTL_VIDEO_CODEC_NAME_VP9;
}

//---------------------------------------------------------------------------
VideoDevice::~VideoDevice()
{
  DecodersMap.clear();
}

//---------------------------------------------------------------------------
unsigned int VideoDevice::GetDeviceContentModifiedEvent() const
{
  return VideoModifiedEvent;
}
  
  
//---------------------------------------------------------------------------
std::string VideoDevice::GetDeviceType() const
{
  return VideoConverter::GetIGTLTypeName();
}

void VideoDevice::SetContent(VideoConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(VideoModifiedEvent, this);
}

VideoConverter::ContentData VideoDevice::GetContent()
{
  return Content;
}


//---------------------------------------------------------------------------
int VideoDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->Copy(buffer);
  if(strcmp(headerMsg->GetDeviceName(), this->GetDeviceName().c_str())==0)
    {
    int returnValue = 0;
    //To Do, we need to unpack the buffer to know the codec type, which is done in the converter
    // So the user need to set the correct CurrentCodecType before hand.
    
    returnValue = VideoConverter::fromIGTL(buffer, &HeaderData, &Content, this->DecodersMap, checkCRC, &this->metaInfo);

    if (returnValue)
     {
     this->Modified();
     this->InvokeEvent(VideoModifiedEvent, this);
     return 1;
     }
  }
 return 0;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer VideoDevice::GetIGTLMessage()
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
  this->OutVideoMessage = igtl::VideoMessage::New();
  //this->OutVideoMessage->AllocateScalars();
#if defined(OpenIGTLink_USE_H264)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_H264) == 0)
    {
    VideoStreamEncoderH264->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    //newEncoder->SetKeyFrameDistance(25);
    VideoStreamEncoderH264->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
    this->OutVideoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_H264);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, &this->OutVideoMessage, VideoStreamEncoderH264, &this->metaInfo);
    }
#endif
#if defined(OpenIGTLink_USE_VP9)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_VP9) == 0)
    {
    VideoStreamEncoderVPX->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    //newEncoder->SetKeyFrameDistance(25);
    VideoStreamEncoderVPX->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
    this->OutVideoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_VP9);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, &this->OutVideoMessage, VideoStreamEncoderVPX, &this->metaInfo);
    }
#endif
#if defined(OpenIGTLink_USE_X265)
  if(this->CurrentCodecType.compare(IGTL_VIDEO_CODEC_NAME_X265) == 0)
    {
    VideoStreamEncoderX265->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    int bitRateFactor = 7;
    VideoStreamEncoderX265->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent)*bitRateFactor);
    this->OutVideoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_X265);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, &this->OutVideoMessage, VideoStreamEncoderX265, &this->metaInfo);
    }
#endif
 if (!iReturn)
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->OutVideoMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer VideoDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
 if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<Device::MESSAGE_PREFIX> VideoDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_NOT_DEFINED);
 return retval;
}

//---------------------------------------------------------------------------
void VideoDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "Video:\t" <<"\n";
  Content.image->PrintSelf(os, indent.GetNextIndent());
}
} // namespace igtlio

