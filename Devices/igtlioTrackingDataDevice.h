/*==========================================================================

Portions (c) Copyright 2019 Robarts Research Institute All Rights Reserved.

==========================================================================*/

#ifndef IGTLIOTDATADEVICE_H
#define IGTLIOTDATADEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioTrackingDataConverter.h"
#include "igtlioDevice.h"

class vtkImageData;

typedef vtkSmartPointer<class igtlioTrackingDataDevice> igtlioTrackingDataDevicePointer;

/// A Device supporting the IMAGE igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT igtlioTrackingDataDevice : public igtlioDevice
{
public:
  enum
  {
    TDATAModifiedEvent          = 118957,
    StartTDATAEvent             = 118970,
    StopTDATAEvent              = 118971
  };

  virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;
  virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
  virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioTrackingDataConverter::ContentData content);
  igtlioTrackingDataConverter::ContentData GetContent();

public:
  static igtlioTrackingDataDevice* New();
  vtkTypeMacro(igtlioTrackingDataDevice, igtlioDevice);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioTrackingDataDevice();
  ~igtlioTrackingDataDevice();

protected:
  igtl::TrackingDataMessage::Pointer OutTDATAMessage;
  igtl::StartTrackingDataMessage::Pointer StartTDATAMessage;
  igtl::StopTrackingDataMessage::Pointer StopTDATAMessage;

  igtlioTrackingDataConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioTrackingDataDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioTrackingDataDeviceCreator* New();
  vtkTypeMacro(igtlioTrackingDataDeviceCreator, vtkObject);
};

#endif //IGTLIOTDATADEVICE_H
