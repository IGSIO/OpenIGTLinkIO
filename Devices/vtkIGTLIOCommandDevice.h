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

class vtkImageData;


//---------------------------------------------------------------------------
class vtkIGTLIOCommandDeviceCreator : public vtkIGTLIODeviceCreator
{
public:
  virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOCommandDeviceCreator *New();
  vtkTypeMacro(vtkIGTLIOCommandDeviceCreator,vtkObject);
};

//---------------------------------------------------------------------------
/// A Device supporting the COMMAND igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOCommandDevice : public vtkIGTLIODevice
{
public:
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(igtl::CommandConverter::ContentData content);
  igtl::CommandConverter::ContentData GetContent();
  std::vector<std::string> GetAvailableCommandNames() const;

 public:
  static vtkIGTLIOCommandDevice *New();
  vtkTypeMacro(vtkIGTLIOCommandDevice,vtkIGTLIODevice);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkIGTLIOCommandDevice();
  ~vtkIGTLIOCommandDevice();

 protected:
  igtl::CommandMessage::Pointer OutMessage;
  igtl::CommandConverter::ContentData Content;
  igtl::CommandConverter::Pointer Converter;
};


#endif // VTKIGTLIOCOMMANDDEVICE_H
