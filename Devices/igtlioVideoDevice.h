/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOVIDEODEVICE_H
#define IGTLIOVIDEODEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioVideoConverter.h"
#include "igtlioDevice.h"


class vtkImageData;

typedef vtkSmartPointer<class igtlioVideoDevice> igtlioVideoDevicePointer;

/// A Device supporting the Video igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT igtlioVideoDevice : public igtlioDevice
{
public:

  enum {
    VideoModifiedEvent         = 118961,
  };

 virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE ;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

 igtl::VideoMessage::Pointer GetCompressedIGTLMessage();

 igtl::VideoMessage::Pointer GetKeyFrameMessage();

  void SetContent(igtlioVideoConverter::ContentData content);

  igtlioVideoConverter::ContentData  GetContent();

  std::string GetCurrentCodecType()
  {
    return CurrentCodecType;
  };

  int SetCurrentCodecType(std::string codecType)
  {
  if (strncmp(codecType.c_str(), IGTL_VIDEO_CODEC_NAME_X265, IGTL_VIDEO_CODEC_NAME_SIZE)==0 ||
      strncmp(codecType.c_str(), IGTL_VIDEO_CODEC_NAME_VP9, IGTL_VIDEO_CODEC_NAME_SIZE)==0 ||
      strncmp(codecType.c_str(), IGTL_VIDEO_CODEC_NAME_H264, IGTL_VIDEO_CODEC_NAME_SIZE)==0 ||
      strncmp(codecType.c_str(), IGTL_VIDEO_CODEC_NAME_OPENHEVC, IGTL_VIDEO_CODEC_NAME_SIZE)==0)
    {
    this->CurrentCodecType = std::string(codecType);
    return 0;
    }
  else
    {
    return -1;
    }
  };


public:
  static igtlioVideoDevice *New();
  vtkTypeMacro(igtlioVideoDevice,igtlioDevice);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioVideoDevice();
  ~igtlioVideoDevice();

 protected:
  igtl::StartVideoMessage::Pointer StartVideoMessage;

  igtl::StopVideoMessage::Pointer StopVideoMessage;

  igtlioVideoConverter::ContentData Content;

  igtl::GenericDecoder* VideoStreamDecoderI420;

  igtl::GenericDecoder* VideoStreamDecoderVPX;

  igtl::GenericDecoder* VideoStreamDecoderX265;

  igtl::GenericDecoder* VideoStreamDecoderH264;

  igtl::GenericDecoder* VideoStreamDecoderAV1;

  std::map<std::string, igtl::GenericDecoder*> DecodersMap;

  igtl::SourcePicture* DecodedPic;

  std::string CurrentCodecType;

  igtl::GenericEncoder* VideoStreamEncoderI420;

  igtl::GenericEncoder* VideoStreamEncoderVPX;

  igtl::GenericEncoder* VideoStreamEncoderX265;

  igtl::GenericEncoder* VideoStreamEncoderH264;

  igtl::GenericEncoder* VideoStreamEncoderAV1;

};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioVideoDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioVideoDeviceCreator *New();
  vtkTypeMacro(igtlioVideoDeviceCreator,vtkObject);
};

#endif //IGTLIOVIDEODEVICE_H
