#include "qIGTLIODevicesWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <qIGTLIOConnectorModel.h>
#include "igtlioLogic.h"
#include "qIGTLIOConnectorPropertyWidget.h"
#include "qIGTLIODevicesModel.h"
#include "qIGTLIODeviceButtonsWidget.h"
#include "qIGTLIODevicePropertiesWidget.h"
#include "vtkIGTLIONode.h"
#include "qIGTLIODeviceAddWidget.h"

//-----------------------------------------------------------------------------
qIGTLIODevicesWidget::qIGTLIODevicesWidget()
{
  DevicesModel = new qIGTLIODevicesModel(this);

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->setMargin(0);

  DevicesListView = new QTreeView;
  DevicesListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  DevicesListView->setModel(DevicesModel);
  topLayout->addWidget(DevicesListView, 1);

  SelectionModel = DevicesListView->selectionModel();
  DevicesModel->setSelectionModel(SelectionModel);
  DevicesListView->setColumnWidth(0, 200);


  ButtonsWidget = new qIGTLIODeviceButtonsWidget();
  topLayout->addWidget(ButtonsWidget);
  ButtonsWidget->setModel(DevicesModel);

  DevicePropertiesWidget = new qIGTLIODevicePropertiesWidget();
  topLayout->addWidget(DevicePropertiesWidget);

  AddDeviceWidget = new qIGTLIODeviceAddWidget();
  topLayout->addWidget(AddDeviceWidget);
  AddDeviceWidget->setModel(DevicesModel);

  connect(SelectionModel, SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(onCurrentDeviceChanged(const QModelIndex&, const QModelIndex&)));
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::onCurrentDeviceChanged(const QModelIndex& current, const QModelIndex& previous)
{
  qIGTLIODevicesModelNode* node = DevicesModel->getNodeFromIndex(current);
  DevicePropertiesWidget->SetNode(node);
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::setLogic(igtlioLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << igtlioLogic::ConnectionAddedEvent
          << igtlioLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionsChanged(vtkObject*, unsigned long, void*, void*)));
    }

  this->Logic = logic;
  DevicesModel->setLogic(logic);
  ButtonsWidget->setLogic(Logic);
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesWidget::onConnectionsChanged(vtkObject* caller, unsigned long event , void* b, void* connector)
{
}


