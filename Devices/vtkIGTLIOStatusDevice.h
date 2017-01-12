#ifndef VTKIGTLIOSTATUSDEVICE_H
#define VTKIGTLIOSTATUSDEVICE_H

// igtlio includes
#include "igtlioDevicesExport.h"

#include "igtlStatusConverter.h"
#include "vtkIGTLIODevice.h"

// OpenIGTLink includes
#include <igtlStatusMessage.h>

// VTK includes
#include <vtkObject.h>

class vtkImageData;

typedef vtkSmartPointer<class vtkIGTLIOStatusDevice> vtkIGTLIOStatusDevicePointer;


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOStatusDeviceCreator : public vtkIGTLIODeviceCreator
{
public:
  virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOStatusDeviceCreator *New();
  vtkTypeMacro(vtkIGTLIOStatusDeviceCreator,vtkObject);
};

//---------------------------------------------------------------------------
/// A Device supporting the STATUS igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOStatusDevice : public vtkIGTLIODevice
{
public:
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(igtlio::StatusConverter::ContentData content);
  igtlio::StatusConverter::ContentData GetContent();

 public:
  static vtkIGTLIOStatusDevice *New();
  vtkTypeMacro(vtkIGTLIOStatusDevice,vtkIGTLIODevice);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkIGTLIOStatusDevice();
  ~vtkIGTLIOStatusDevice();

 protected:
  igtl::StatusMessage::Pointer OutMessage;
  igtl::GetStatusMessage::Pointer GetMessage;

  igtlio::StatusConverter::ContentData Content;
};


#endif // VTKIGTLIOSTATUSDEVICE_H
