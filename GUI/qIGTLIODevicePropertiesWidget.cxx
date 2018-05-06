#include "qIGTLIODevicePropertiesWidget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include "vtkIGTLIONode.h"
#include <QLabel>
#include "qIGTLIODeviceWidgetFactory.h"
#include "qIGTLIODeviceWidget.h"

class WidgetInGroupBoxClass
{
public:
  WidgetInGroupBoxClass(QVBoxLayout* parentLayout, QString header)
  {
    groupBox = new QGroupBox(header);
    parentLayout->addWidget(groupBox);
    layout = new QVBoxLayout(groupBox);
    layout->setMargin(0);
  }

  void replaceWidget(QWidget* newWidget)
  {
    this->clearLayout();
    if (newWidget)
      layout->addWidget(newWidget);
  }
  void clearLayout()
  {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != 0)
    {
      QWidget* widget = child->widget();
      delete child;
      delete widget;
    }
  }

protected:
  QGroupBox* groupBox;
  QVBoxLayout* layout;
};


qIGTLIODevicePropertiesWidget::qIGTLIODevicePropertiesWidget(QWidget* parent) : QWidget(parent)
{
  DeviceWidgetFactory = vtkIGTLIODeviceWidgetFactoryPointer::New();

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);

  WidgetInGroupBox = new WidgetInGroupBoxClass(layout, "Device");
}

void qIGTLIODevicePropertiesWidget::SetNode(qIGTLIODevicesModelNode *node)
{
  Node = node;

  igtlioDevice* device = Node->device;

  qIGTLIODeviceWidget* widget = NULL;

  if (device)
    {
      widget = DeviceWidgetFactory->create(device->GetDeviceType());
      widget->SetDevice(device);
    }

  WidgetInGroupBox->replaceWidget(widget);
}

