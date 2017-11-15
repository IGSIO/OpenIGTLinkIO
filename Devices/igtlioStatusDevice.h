#ifndef IGTLIOSTATUSDEVICE_H
#define IGTLIOSTATUSDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioStatusConverter.h"
#include "igtlioDevice.h"

namespace igtlio
{

typedef vtkSmartPointer<class StatusDevice> StatusDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT StatusDevice : public Device
{
public:
  enum {
    StatusModifiedEvent         = 118956,
  };
  
 virtual vtkIntArray* GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(StatusConverter::ContentData content);
  StatusConverter::ContentData GetContent();

 public:
  static StatusDevice *New();
  vtkTypeMacro(StatusDevice,Device);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 protected:
  StatusDevice();
  ~StatusDevice();

 protected:
  igtl::StatusMessage::Pointer OutMessage;
  igtl::GetStatusMessage::Pointer GetMessage;

  StatusConverter::ContentData Content;
};


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT StatusDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static StatusDeviceCreator *New();
  vtkTypeMacro(StatusDeviceCreator,vtkObject);
};

} // namespace igtlio


#endif // IGTLIOSTATUSDEVICE_H
