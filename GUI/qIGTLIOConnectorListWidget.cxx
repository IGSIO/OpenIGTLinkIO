#include "qIGTLIOConnectorListWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <qIGTLIOConnectorModel.h>
#include "vtkIGTLIOLogic.h"
#include "qIGTLIOConnectorPropertyWidget.h"

//-----------------------------------------------------------------------------
qIGTLIOConnectorListWidget::qIGTLIOConnectorListWidget()
{
  ConnectorModel = new qIGTLIOConnectorModel(this);

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->setMargin(0);

  ConnectorListView = new QTreeView;
  ConnectorListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ConnectorListView->setModel(ConnectorModel);
  topLayout->addWidget(ConnectorListView);

  SelectionModel = ConnectorListView->selectionModel();

  this->addButtonFrame(topLayout);

  ConnectorPropertyWidget = new qIGTLIOConnectorPropertyWidget(this);
  topLayout->addWidget(ConnectorPropertyWidget);
  connect(SelectionModel, SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(onCurrentConnectorChanged()));
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onCurrentConnectorChanged()
{
  int row = SelectionModel->currentIndex().row();

  if (row<0 || row>=Logic->GetNumberOfConnectors())
    {
    return;
    }

  vtkIGTLIOConnectorPointer connector = Logic->GetConnector(row);
  ConnectorPropertyWidget->setMRMLIGTLConnectorNode(connector);
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::addButtonFrame(QVBoxLayout* topLayout)
{
  QFrame* buttonFrame = new QFrame;
  buttonFrame->setFrameShape(QFrame::NoFrame);
  buttonFrame->setFrameShadow(QFrame::Plain);
  topLayout->addWidget(buttonFrame);
  QHBoxLayout* buttonLayout = new QHBoxLayout(buttonFrame);
  buttonLayout->setMargin(0);

  QPushButton* addConnectorButton = new QPushButton("+");
  buttonLayout->addWidget(addConnectorButton);
  QPushButton* removeConnectorButton = new QPushButton("--");
  buttonLayout->addWidget(removeConnectorButton);
  QPushButton* debugUpdateConnectorButton = new QPushButton("Update");
  buttonLayout->addWidget(debugUpdateConnectorButton);
  buttonLayout->addStretch();

  //  Add(+) / Remove(-) Connector Buttons
  connect(addConnectorButton, SIGNAL(clicked()), this,
          SLOT(onAddConnectorButtonClicked()));
  connect(removeConnectorButton, SIGNAL(clicked()), this,
          SLOT(onRemoveConnectorButtonClicked()));
  connect(debugUpdateConnectorButton, SIGNAL(clicked()), this,
          SLOT(onDebugUpdateButtonClicked()));
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::setLogic(vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOLogic::ConnectionAddedEvent
          << vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionsChanged(vtkObject*, void*, unsigned long, void*)));
    }

  this->Logic = logic;
  ConnectorModel->setLogic(logic);
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onConnectionsChanged(vtkObject* caller, void* connector, unsigned long event , void* b)
{
  // remove removed connector from property widget
  if (event==vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent && connector!=NULL)
    {
      vtkIGTLIOConnector* c = static_cast<vtkIGTLIOConnector*>(connector);
      if (ConnectorPropertyWidget->getMRMLIGTLConnectorNode().GetPointer() == c)
        ConnectorPropertyWidget->setMRMLIGTLConnectorNode(vtkIGTLIOConnectorPointer());
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onAddConnectorButtonClicked()
{
  Logic->CreateConnector();
  this->selectRow(Logic->GetNumberOfConnectors()-1);
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::selectRow(int row)
{
  if (Logic->GetNumberOfConnectors()==0)
    {
      return;
    }

  row = std::max(row, 0);
  row = std::min(row, Logic->GetNumberOfConnectors()-1);
  QModelIndex newIndex = ConnectorModel->index(row, 0);

  ConnectorListView->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::Select);
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onRemoveConnectorButtonClicked()
{
  if (!ConnectorListView->selectionModel()->currentIndex().isValid())
    return;
  int row = ConnectorListView->selectionModel()->currentIndex().row();
  Logic->RemoveConnector(row);
  this->selectRow(row);
}

void qIGTLIOConnectorListWidget::onDebugUpdateButtonClicked()
{
  ConnectorModel->resetModel();
}

