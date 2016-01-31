#include "qIGTLIOClientWidget.h"

#include <QVBoxLayout>
#include "qIGTLIOConnectorListWidget.h"

qIGTLIOClientWidget::qIGTLIOClientWidget()
{
  QVBoxLayout* layout = new QVBoxLayout(this);
//  layout->setMargin(0);

  ConnectorListWidget = new qIGTLIOConnectorListWidget;
  layout->addWidget(ConnectorListWidget);
}

void qIGTLIOClientWidget::setLogic(vtkIGTLIOLogicPointer logic)
{
  this->Logic = logic;
  ConnectorListWidget->setLogic(Logic);
}


