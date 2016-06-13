#include "qIGTLIODeviceWidget.h"

#include "vtkIGTLIODevice.h"

qIGTLIODeviceWidget::qIGTLIODeviceWidget(QWidget *parent) : QWidget(parent)
{

}

void qIGTLIODeviceWidget::SetDevice(vtkSmartPointer<vtkIGTLIODevice> device)
{
  qvtkReconnect(Device, device, vtkCommand::ModifiedEvent,
                this, SLOT(onDeviceModified()));

  Device = device;

  this->setupUi();
  this->onDeviceModified();
}
