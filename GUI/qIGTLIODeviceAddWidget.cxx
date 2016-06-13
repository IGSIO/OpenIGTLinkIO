#include "qIGTLIODeviceAddWidget.h"

#include "vtkIGTLIOLogic.h"
#include "qIGTLIODevicesModel.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QItemSelectionModel>
#include "vtkIGTLIONode.h"

qIGTLIODeviceAddWidget::qIGTLIODeviceAddWidget()
{
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->setMargin(0);

  QFrame* buttonFrame = new QFrame;
  buttonFrame->setFrameShape(QFrame::NoFrame);
  buttonFrame->setFrameShadow(QFrame::Plain);
  topLayout->addWidget(buttonFrame);
  QHBoxLayout* buttonLayout = new QHBoxLayout(buttonFrame);
  buttonLayout->setMargin(0);

  mAddDeviceAction = new QAction("Add Device", this);
  connect(mAddDeviceAction, SIGNAL(triggered()), this, SLOT(onAddDevice()));
  QToolButton* button = new QToolButton(this);
  button->setDefaultAction(mAddDeviceAction);
  buttonLayout->addWidget(button);

  mSelectDeviceType = new QComboBox;
  buttonLayout->addWidget(mSelectDeviceType);

  mDeviceName = new QLineEdit;
  mDeviceName->setText("<broadcast>");
  mDeviceName->setToolTip("Enter device name for new device, or empty for a broadcast");
  buttonLayout->addWidget(mDeviceName);

  buttonLayout->addStretch(1);

  this->onCurrentConnectorChanged();
}

void qIGTLIODeviceAddWidget::setLogic(vtkIGTLIOLogicPointer logic)
{
  Logic = logic;
}

void qIGTLIODeviceAddWidget::setModel(qIGTLIODevicesModel *model)
{
  DevicesModel = model;
  connect(DevicesModel->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(onCurrentConnectorChanged()));
  this->onCurrentConnectorChanged();
}

qIGTLIODevicesModelNode* qIGTLIODeviceAddWidget::getSelectedNode()
{
  if (DevicesModel.isNull())
    return NULL;

  QModelIndex index = DevicesModel->selectionModel()->currentIndex();
  qIGTLIODevicesModelNode* node = DevicesModel->getNodeFromIndex(index);
  return node;
}

void qIGTLIODeviceAddWidget::onCurrentConnectorChanged()
{
  qIGTLIODevicesModelNode* node = this->getSelectedNode();

  this->setEnabled(node);

  if (!node)
    return;

  int index = mSelectDeviceType->currentIndex();
  mSelectDeviceType->clear();
  std::vector<std::string> availableDeviceTypes = node->connector->GetDeviceFactory()->GetAvailableDeviceTypes();
  for (unsigned i=0; i<availableDeviceTypes.size(); ++i)
    {
    mSelectDeviceType->addItem(availableDeviceTypes[i].c_str());
    }


  if (index<0)
    index = 0;
  if (index >= mSelectDeviceType->count())
    {
    index = mSelectDeviceType->count() -1;
    }
  mSelectDeviceType->setCurrentIndex(index);
}

void qIGTLIODeviceAddWidget::onAddDevice()
{
  qIGTLIODevicesModelNode* node = this->getSelectedNode();

  if (!node)
    return;

  std::string deviceType = mSelectDeviceType->currentText().toStdString();
  std::string deviceName = mDeviceName->text().toStdString();
  if (deviceName=="<broadcast>")
    deviceName = "";

  if (!node->connector->GetDevice(deviceName))
    {
      qIGTLIODevicesModelNode* node = this->getSelectedNode();
      vtkIGTLIODevicePointer device = node->connector->GetDeviceFactory()->create(deviceType, deviceName);
      device->SetMessageDirection(vtkIGTLIODevice::MESSAGE_DIRECTION_OUT);
      node->connector->AddDevice(device);
    }

  // TODO select the device, even if it was not created. Expand view to see it.
}

