#ifndef STRINGDEVICE_H
#define STRINGDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioStringConverter.h"
#include "igtlioDevice.h"

class vtkImageData;

typedef vtkSmartPointer<class igtlioStringDevice> igtlioStringDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioStringDevice : public igtlioDevice
{
public:
  enum {
    StringModifiedEvent         = 118960,
  };

 virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioStringConverter::ContentData content);
  igtlioStringConverter::ContentData GetContent();

 public:
  static igtlioStringDevice *New();
  vtkTypeMacro(igtlioStringDevice,igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 protected:
  igtlioStringDevice();
  ~igtlioStringDevice();

 protected:
  igtl::StringMessage::Pointer OutMessage;
  //igtl::GetStatusMessage::Pointer GetMessage;

  igtlioStringConverter::ContentData Content;
};


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioStringDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioStringDeviceCreator *New();
  vtkTypeMacro(igtlioStringDeviceCreator,vtkObject);
};

#endif // STRINGDEVICE_H
