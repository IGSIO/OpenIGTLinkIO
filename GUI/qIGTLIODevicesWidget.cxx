#include "qIGTLIODevicesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <qIGTLIOConnectorModel.h>
#include "vtkIGTLIOLogic.h"
#include "qIGTLIOConnectorPropertyWidget.h"
#include "qIGTLIODevicesModel.h"

//-----------------------------------------------------------------------------
qIGTLIODevicesWidget::qIGTLIODevicesWidget()
{
  DevicesModel = new qIGTLIODevicesModel(this);

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->setMargin(0);

  DevicesListView = new QTreeView;
  DevicesListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  DevicesListView->setModel(DevicesModel);
  topLayout->addWidget(DevicesListView);

  SelectionModel = DevicesListView->selectionModel();

//  this->addButtonFrame(topLayout);
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::onCurrentConnectorChanged()
{
//  int row = SelectionModel->currentIndex().row();

//  if (row<0 || row>=Logic->GetNumberOfConnectors())
//    {
//    return;
//    }

//  vtkIGTLIOConnectorPointer connector = Logic->GetConnector(row);
//  ConnectorPropertyWidget->setMRMLIGTLConnectorNode(connector);
}

////-----------------------------------------------------------------------------
//void qIGTLIODevicesWidget::addButtonFrame(QVBoxLayout* topLayout)
//{
//  QFrame* buttonFrame = new QFrame;
//  buttonFrame->setFrameShape(QFrame::NoFrame);
//  buttonFrame->setFrameShadow(QFrame::Plain);
//  topLayout->addWidget(buttonFrame);
//  QHBoxLayout* buttonLayout = new QHBoxLayout(buttonFrame);
//  buttonLayout->setMargin(0);

//  QPushButton* addConnectorButton = new QPushButton("+");
//  buttonLayout->addWidget(addConnectorButton);
//  QPushButton* removeConnectorButton = new QPushButton("--");
//  buttonLayout->addWidget(removeConnectorButton);
//  QPushButton* debugUpdateConnectorButton = new QPushButton("Update");
//  buttonLayout->addWidget(debugUpdateConnectorButton);
//  buttonLayout->addStretch();

//  //  Add(+) / Remove(-) Connector Buttons
//  connect(addConnectorButton, SIGNAL(clicked()), this,
//          SLOT(onAddConnectorButtonClicked()));
//  connect(removeConnectorButton, SIGNAL(clicked()), this,
//          SLOT(onRemoveConnectorButtonClicked()));
//  connect(debugUpdateConnectorButton, SIGNAL(clicked()), this,
//          SLOT(onDebugUpdateButtonClicked()));
//}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::setLogic(vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOLogic::ConnectionAddedEvent
          << vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionsChanged(vtkObject*, void*, unsigned long, void*)));
    }

  this->Logic = logic;
  DevicesModel->setLogic(logic);
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::onConnectionsChanged(vtkObject* caller, void* connector, unsigned long event , void* b)
{
//  // remove removed connector from property widget
//  if (event==vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent && connector!=NULL)
//    {
//      vtkIGTLIOConnector* c = static_cast<vtkIGTLIOConnector*>(connector);
//      if (ConnectorPropertyWidget->getMRMLIGTLConnectorNode().GetPointer() == c)
//        ConnectorPropertyWidget->setMRMLIGTLConnectorNode(vtkIGTLIOConnectorPointer());
//    }
}


