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

namespace igtlio
{

typedef vtkSmartPointer<class VideoDevice> VideoDevicePointer;

/// A Device supporting the Video igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT VideoDevice : public Device
{
public:
  
  enum {
    VideoModifiedEvent         = 118958,
  };
  
  enum CodecTypes
  {
    useVP9 = 1,
    useH265 = 2,
    useH264 = 3
  };
  
 virtual vtkIntArray* GetDeviceContentModifiedEvent() const;
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;
  
  void SetContent(VideoConverter::ContentData content);
  
  VideoConverter::ContentData  GetContent();
  
  int GetCurrentCodecType()
  {
    return currentCodecType;
  };
  
  void SetCurrentCodecType(CodecTypes type)
  {
    this->currentCodecType = type;
  };
  

public:
  static VideoDevice *New();
  vtkTypeMacro(VideoDevice,Device);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  VideoDevice();
  ~VideoDevice();

 protected:
  igtl::VideoMessage::Pointer OutVideoMessage;
  igtl::StartVideoDataMessage::Pointer GetVideoMessage;

  VideoConverter::ContentData Content;
  
  GenericDecoder* VideoStreamDecoderVPX;
  
  GenericDecoder* VideoStreamDecoderX265;
  
  GenericDecoder* VideoStreamDecoderH264;
  
  SourcePicture* pDecodedPic;
  
  CodecTypes currentCodecType;
  
  GenericEncoder* VideoStreamEncoderVPX;
  
  GenericEncoder* VideoStreamEncoderX265;
  
  GenericEncoder* VideoStreamEncoderH264;
  
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT VideoDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static VideoDeviceCreator *New();
  vtkTypeMacro(VideoDeviceCreator,vtkObject);
};

} //namespace igtlio

#endif //IGTLIOVIDEODEVICE_H
