#include "qIGTLIOClientWidget.h"

#include <QVBoxLayout>
#include <QSplitter>
#include "qIGTLIOConnectorListWidget.h"
#include "qIGTLIODevicesWidget.h"
#include "qIGTLIOCommandWidget.h"
#include "igtlioLogic.h"

qIGTLIOClientWidget::qIGTLIOClientWidget()
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  QSplitter* splitter = new QSplitter(this);
  //  splitter->setOrientation(Qt::Vertical);
  layout->addWidget(splitter);

  ConnectorListWidget = new qIGTLIOConnectorListWidget;
  splitter->addWidget(ConnectorListWidget);

  DevicesWidget = new qIGTLIODevicesWidget;
  splitter->addWidget(DevicesWidget);

  CommandWidget = new qIGTLIOCommandWidget;
  splitter->addWidget(CommandWidget);
}

void qIGTLIOClientWidget::setLogic(igtlioLogicPointer logic)
{
  this->Logic = logic;
  ConnectorListWidget->setLogic(Logic);
  DevicesWidget->setLogic(Logic);
  CommandWidget->setLogic(Logic);
}


