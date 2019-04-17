#ifndef POINTDEVICE_H
#define POINTDEVICE_H

// OpenIGTLinkIO includes
#include "igtlioDevice.h"
#include "igtlioDevicesExport.h"
#include "igtlioPointConverter.h"

typedef vtkSmartPointer<class igtlioPointDevice> igtlioPointDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioPointDevice : public igtlioDevice
{
public:
  enum {
    PointModifiedEvent = 118964,
  };

  virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;
  virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
  virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioPointConverter::ContentData content);
  igtlioPointConverter::ContentData GetContent();

public:
  static igtlioPointDevice *New();
  vtkTypeMacro(igtlioPointDevice, igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioPointDevice();
  ~igtlioPointDevice();

protected:
  igtl::PointMessage::Pointer OutMessage;

  igtlioPointConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioPointDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioPointDeviceCreator *New();
  vtkTypeMacro(igtlioPointDeviceCreator, vtkObject);
};

#endif // POINTDEVICE_H
