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
#if OpenIGTLink_LINK_VP9
    VideoStreamDecoderVPX = new VP9Decoder();
    VideoStreamEncoderVPX = new VP9Encoder();
#endif
#if OpenIGTLink_LINK_X265
    VideoStreamDecoderX265 = new H265Decoder();
    VideoStreamEncoderX265 = new H265Encoder();
#endif
#if OpenIGTLink_LINK_H264
    VideoStreamDecoderH264 = new H264Decoder();
    VideoStreamEncoderH264 = new H264Encoder();
#endif
  pDecodedPic = new SourcePicture();
  this->currentCodecType = useVP9;
}

//---------------------------------------------------------------------------
VideoDevice::~VideoDevice()
{
}

//---------------------------------------------------------------------------
vtkIntArray* VideoDevice::GetDeviceContentModifiedEvent() const
{
  vtkIntArray* events;
  events = vtkIntArray::New();
  events->InsertNextValue(VideoModifiedEvent);
  return events;
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
    // So the user need to set the correct currentCodecType before hand.
    if (this->currentCodecType == useH264 && OpenIGTLink_LINK_H264)
    {
      returnValue = VideoConverter::fromIGTL(buffer, &HeaderData, &Content, VideoStreamDecoderH264, checkCRC);
    }
    else if(this->currentCodecType == useVP9 && OpenIGTLink_LINK_VP9)
    {
      returnValue = VideoConverter::fromIGTL(buffer, &HeaderData, &Content, VideoStreamDecoderVPX, checkCRC);
    }
    else if(this->currentCodecType == useH265 && OpenIGTLink_LINK_X265)
    {
      returnValue = VideoConverter::fromIGTL(buffer, &HeaderData, &Content,VideoStreamDecoderX265, checkCRC);
    }
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
#ifdef OpenIGTLink_LINK_VP9
  if(this->useVP9)
  {
    VideoStreamEncoderVPX->SetPicWidthAndHeight(imageSizePixels[0], imageSizePixels[1]);
    //newEncoder->SetKeyFrameDistance(25);
    VideoStreamEncoderVPX->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent));
    VideoStreamEncoderVPX->InitializeEncoder();
    VideoStreamEncoderVPX->SetLosslessLink(true);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, &this->OutVideoMessage, VideoStreamEncoderVPX);
  }
#endif
#ifdef OpenIGTLink_LINK_H265
  if(this->useH265)
  {
    VideoStreamEncoderX265->SetPicWidthAndHeight(trackedFrame.GetFrameSize()[0], trackedFrame.GetFrameSize()[1]);
    int bitRateFactor = 7;
    VideoStreamEncoderX265->SetLosslessLink(true);
    VideoStreamEncoderX265->SetRCTaregetBitRate((int)(imageSizePixels[0] * imageSizePixels[1] * 8 * frameRate * bitRatePercent)*bitRateFactor);
    VideoStreamEncoderX265->InitializeEncoder();
    VideoStreamEncoderX265->SetSpeed(9);
    iReturn = VideoConverter::toIGTL(HeaderData, Content, &this->OutVideoMessage, VideoStreamEncoderX265)
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
 if (prefix==MESSAGE_PREFIX_START)
  {
   if (this->StartVideoMessage.IsNull())
     {
     this->StartVideoMessage = igtl::StartVideoMessage::New();
     }
   this->StartVideoMessage->SetDeviceName(HeaderData.deviceName.c_str());
   this->StartVideoMessage->Pack();
   return dynamic_pointer_cast<igtl::MessageBase>(this->StartVideoMessage);
  }
 if (prefix==MESSAGE_PREFIX_STOP)
  {
    if (this->StopVideoMessage.IsNull())
    {
      this->StopVideoMessage = igtl::StopVideoMessage::New();
    }
    this->StopVideoMessage->SetDeviceName(HeaderData.deviceName.c_str());
    this->StopVideoMessage->Pack();
    return dynamic_pointer_cast<igtl::MessageBase>(this->StopVideoMessage);
  }
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
 retval.insert(MESSAGE_PREFIX_START);
 retval.insert(MESSAGE_PREFIX_STOP);
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

