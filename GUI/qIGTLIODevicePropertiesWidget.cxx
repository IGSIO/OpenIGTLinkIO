#include "qIGTLIODevicePropertiesWidget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include "vtkIGTLIONode.h"
#include <QLabel>
#include "qIGTLIODeviceWidgetFactory.h"
#include "qIGTLIODeviceWidget.h"

struct WidgetInGroupBoxClass
{
  WidgetInGroupBoxClass(QVBoxLayout* parentLayout, QString header)
  {
    groupBox = new QGroupBox(header);
    parentLayout->addWidget(groupBox);
    layout = new QVBoxLayout(groupBox);
    layout->setMargin(2);
  }

  QGroupBox* groupBox;
  QVBoxLayout* layout;

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
};


qIGTLIODevicePropertiesWidget::qIGTLIODevicePropertiesWidget(QWidget* parent) : QWidget(parent)
{
  DeviceWidgetFactory = vtkIGTLIODeviceWidgetFactoryPointer::New();

  QVBoxLayout* layout = new QVBoxLayout(this);

  WidgetInGroupBox = new WidgetInGroupBoxClass(layout, "Device");
}

void qIGTLIODevicePropertiesWidget::SetNode(qIGTLIODevicesModelNode *node)
{
  Node = node;

  vtkIGTLIODevice* device = Node->device;

  qIGTLIODeviceWidget* widget = NULL;

  if (device)
    {
      widget = DeviceWidgetFactory->create(device->GetDeviceType());
      widget->SetDevice(device);
    }

  WidgetInGroupBox->replaceWidget(widget);
}

