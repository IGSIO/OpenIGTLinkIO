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
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::onCurrentConnectorChanged()
{
}

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
}


