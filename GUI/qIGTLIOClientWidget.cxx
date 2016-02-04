#include "qIGTLIOClientWidget.h"

#include <QVBoxLayout>
#include "qIGTLIOConnectorListWidget.h"
#include "qIGTLIODevicesWidget.h"

qIGTLIOClientWidget::qIGTLIOClientWidget()
{
  QVBoxLayout* layout = new QVBoxLayout(this);
//  layout->setMargin(0);

  ConnectorListWidget = new qIGTLIOConnectorListWidget;
  layout->addWidget(ConnectorListWidget);

  DevicesWidget = new qIGTLIODevicesWidget;
  layout->addWidget(DevicesWidget);
}

void qIGTLIOClientWidget::setLogic(vtkIGTLIOLogicPointer logic)
{
  this->Logic = logic;
  ConnectorListWidget->setLogic(Logic);
  DevicesWidget->setLogic(Logic);
}


