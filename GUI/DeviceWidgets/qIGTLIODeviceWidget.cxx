#include "qIGTLIODeviceWidget.h"

#include "igtlioDevice.h"

qIGTLIODeviceWidget::qIGTLIODeviceWidget(QWidget *parent) : QWidget(parent)
{

}

void qIGTLIODeviceWidget::SetDevice(igtlioDevicePointer device)
{
  qvtkReconnect(Device, device, vtkCommand::ModifiedEvent,
                this, SLOT(onDeviceModified()));

  Device = device;

  this->setupUi();
  this->onDeviceModified();
}
