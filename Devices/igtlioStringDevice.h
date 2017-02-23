#ifndef STRINGDEVICE_H
#define STRINGDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioStringConverter.h"
#include "igtlioDevice.h"

namespace igtlio
{

class vtkImageData;
typedef vtkSmartPointer<class StringDevice> StringDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT StringDevice : public Device
{
public:
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(StringConverter::ContentData content);
  StringConverter::ContentData GetContent();

 public:
  static StringDevice *New();
  vtkTypeMacro(StringDevice,Device);

  void PrintSelf(ostream& os, vtkIndent indent);

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
  virtual DevicePointer Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static StringDeviceCreator *New();
  vtkTypeMacro(StringDeviceCreator,vtkObject);
};

} // namespace igtlio

#endif // STRINGDEVICE_H
