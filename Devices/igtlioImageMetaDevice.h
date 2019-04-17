#ifndef IMAGEMETADEVICE_H
#define IMAGEMETADEVICE_H

// OpenIGTLinkIO includes
#include "igtlioDevice.h"
#include "igtlioDevicesExport.h"
#include "igtlioImageMetaConverter.h"

typedef vtkSmartPointer<class igtlioImageMetaDevice> igtlioImageMetaDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioImageMetaDevice : public igtlioDevice
{
public:
  enum {
    ImageMetaModifiedEvent = 118962,
  };

  virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;
  virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
  virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioImageMetaConverter::ContentData content);
  igtlioImageMetaConverter::ContentData GetContent();

public:
  static igtlioImageMetaDevice *New();
  vtkTypeMacro(igtlioImageMetaDevice, igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioImageMetaDevice();
  ~igtlioImageMetaDevice();

protected:
  igtl::ImageMetaMessage::Pointer OutMessage;

  igtlioImageMetaConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioImageMetaDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioImageMetaDeviceCreator *New();
  vtkTypeMacro(igtlioImageMetaDeviceCreator, vtkObject);
};

#endif // IMAGEMETADEVICE_H
