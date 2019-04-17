#ifndef LABELMETADEVICE_H
#define LABELMETADEVICE_H

// OpenIGTLinkIO includes
#include "igtlioDevice.h"
#include "igtlioDevicesExport.h"
#include "igtlioLabelMetaConverter.h"

typedef vtkSmartPointer<class igtlioLabelMetaDevice> igtlioLabelMetaDevicePointer;

class OPENIGTLINKIO_DEVICES_EXPORT igtlioLabelMetaDevice : public igtlioDevice
{
public:
  enum {
    LabelMetaModifiedEvent = 118963,
  };

  virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;
  virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
  virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
  virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioLabelMetaConverter::ContentData content);
  igtlioLabelMetaConverter::ContentData GetContent();

public:
  static igtlioLabelMetaDevice *New();
  vtkTypeMacro(igtlioLabelMetaDevice, igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioLabelMetaDevice();
  ~igtlioLabelMetaDevice();

protected:
  igtl::LabelMetaMessage::Pointer OutMessage;

  igtlioLabelMetaConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioLabelMetaDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioLabelMetaDeviceCreator *New();
  vtkTypeMacro(igtlioLabelMetaDeviceCreator, vtkObject);
};

#endif // LABELMETADEVICE_H
