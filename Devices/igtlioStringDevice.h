#ifndef STRINGDEVICE_H
#define STRINGDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioStringConverter.h"
#include "igtlioDevice.h"

class vtkImageData;
namespace igtlio
{

typedef vtkSmartPointer<class StringDevice> StringDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT StringDevice : public Device
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

  void SetContent(StringConverter::ContentData content);
  StringConverter::ContentData GetContent();

 public:
  static StringDevice *New();
  vtkTypeMacro(StringDevice,Device);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 protected:
  StringDevice();
  ~StringDevice();

 protected:
  igtl::StringMessage::Pointer OutMessage;
  //igtl::GetStatusMessage::Pointer GetMessage;

  StringConverter::ContentData Content;
};


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT StringDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static StringDeviceCreator *New();
  vtkTypeMacro(StringDeviceCreator,vtkObject);
};

} // namespace igtlio

#endif // STRINGDEVICE_H
