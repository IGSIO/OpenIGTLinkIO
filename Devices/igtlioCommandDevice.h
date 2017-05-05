#ifndef IGTLIOCOMMANDDEVICE_H
#define IGTLIOCOMMANDDEVICE_H

#include "igtlioDevicesExport.h"
#include "igtlioCommandConverter.h"
#include "igtlioDevice.h"

namespace igtlio
{
typedef vtkSmartPointer<class CommandDevice> CommandDevicePointer;

/// A Device supporting the COMMAND igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT CommandDevice : public Device
{
public:
  enum {
    CommandModifiedEvent         = 118958,
  };
  
 virtual vtkIntArray* GetDeviceContentModifiedEvent() const;
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(CommandConverter::ContentData content);
  CommandConverter::ContentData GetContent();
  std::vector<std::string> GetAvailableCommandNames() const;

  igtl::MessageBase::Pointer GetIGTLResponseMessage();
  CommandDevicePointer GetResponseFromCommandID(int id);

 public:
  static CommandDevice *New();
  vtkTypeMacro(CommandDevice,Device);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  CommandDevice();
  ~CommandDevice();

 protected:
  igtl::CommandMessage::Pointer OutMessage;
  igtl::RTSCommandMessage::Pointer ResponseMessage;
  CommandConverter::ContentData Content;
};

//---------------------------------------------------------------------------

class OPENIGTLINKIO_DEVICES_EXPORT CommandDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static CommandDeviceCreator *New();
  vtkTypeMacro(CommandDeviceCreator,vtkObject);
};

} // namespace igtlio

#endif // IGTLIOCOMMANDDEVICE_H
