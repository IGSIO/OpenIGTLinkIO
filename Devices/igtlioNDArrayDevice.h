#ifndef NDARRAYDEVICE_H
#define NDARRAYDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioNDArrayConverter.h"
#include "igtlioDevice.h"

class vtkDataArray;

typedef vtkSmartPointer<class igtlioNDArrayDevice> igtlioNDArrayDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioNDArrayDevice : public igtlioDevice
{
public:
  enum {
    ArrayModifiedEvent         = 118960,
  };

 virtual unsigned int GetDeviceContentModifiedEvent() const;
 virtual std::string GetDeviceType() const;
 //virtual int ReceiveIGTLMessage(igtl::NDArrayMessage::Pointer buffer, bool checkCRC);
 virtual igtl::NDArrayMessage::Pointer GetIGTLMessage() const;
 virtual igtl::NDArrayMessage::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) const;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes();

  void SetContent(igtlioNDArrayConverter::ContentData content);
  igtlioNDArrayConverter::ContentData GetContent();

 public:
  static igtlioNDArrayDevice *New();
  vtkTypeMacro(igtlioNDArrayDevice,igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  igtlioNDArrayDevice();
  ~igtlioNDArrayDevice();

 protected:
  igtl::NDArrayMessage::Pointer OutMessage;
  //igtl::GetStatusMessage::Pointer GetMessage;

  igtlioNDArrayConverter::ContentData Content;
};


//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioNDArrayDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name);
  virtual std::string GetDeviceType();

  static igtlioNDArrayDeviceCreator *New();
  vtkTypeMacro(igtlioNDArrayDeviceCreator,vtkObject);
};

#endif // NDARRAYDEVICE_H
