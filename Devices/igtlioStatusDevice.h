#ifndef IGTLIOSTATUSDEVICE_H
#define IGTLIOSTATUSDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioStatusConverter.h"
#include "igtlioDevice.h"

typedef vtkSmartPointer<class igtlioStatusDevice> igtlioStatusDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioStatusDevice : public igtlioDevice
{
public:
  enum {
    StatusModifiedEvent         = 118956,
  };

 virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioStatusConverter::ContentData content);
  igtlioStatusConverter::ContentData GetContent();

 public:
  static igtlioStatusDevice *New();
  vtkTypeMacro(igtlioStatusDevice,igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 protected:
  igtlioStatusDevice();
  ~igtlioStatusDevice();

 protected:
  igtl::StatusMessage::Pointer OutMessage;
  igtl::GetStatusMessage::Pointer GetMessage;

  igtlioStatusConverter::ContentData Content;
};


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioStatusDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioStatusDeviceCreator *New();
  vtkTypeMacro(igtlioStatusDeviceCreator,vtkObject);
};

#endif // IGTLIOSTATUSDEVICE_H
