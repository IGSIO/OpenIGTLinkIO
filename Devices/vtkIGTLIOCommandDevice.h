#ifndef VTKIGTLIOCOMMANDDEVICE_H
#define VTKIGTLIOCOMMANDDEVICE_H

#include <vector>

// igtlio includes
#include "igtlioDevicesExport.h"

#include "igtlCommandConverter.h"
#include "vtkIGTLIODevice.h"

// OpenIGTLink includes
#include <igtlCommandMessage.h>

// VTK includes
#include <vtkObject.h>

//class vtkImageData;

namespace igtlio
{
typedef vtkSmartPointer<class vtkIGTLIOCommandDevice> vtkIGTLIOCommandDevicePointer;

/// A Device supporting the COMMAND igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOCommandDevice : public vtkIGTLIODevice
{
public:
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(igtlio::CommandConverter::ContentData content);
  igtlio::CommandConverter::ContentData GetContent();
  std::vector<std::string> GetAvailableCommandNames() const;

  igtl::MessageBase::Pointer GetIGTLResponseMessage();
  vtkIGTLIOCommandDevicePointer GetResponseFromCommandID(int id);

 public:
  static vtkIGTLIOCommandDevice *New();
  vtkTypeMacro(vtkIGTLIOCommandDevice,vtkIGTLIODevice);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkIGTLIOCommandDevice();
  ~vtkIGTLIOCommandDevice();

 protected:
  igtl::CommandMessage::Pointer OutMessage;
  igtl::RTSCommandMessage::Pointer ResponseMessage;
  igtlio::CommandConverter::ContentData Content;
};

//---------------------------------------------------------------------------

class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOCommandDeviceCreator : public vtkIGTLIODeviceCreator
{
public:
  virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOCommandDeviceCreator *New();
  vtkTypeMacro(vtkIGTLIOCommandDeviceCreator,vtkObject);
};

} // namespace igtlio

#endif // VTKIGTLIOCOMMANDDEVICE_H
