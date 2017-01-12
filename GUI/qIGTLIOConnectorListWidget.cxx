#include "qIGTLIOConnectorListWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QTreeView>
#include <qIGTLIOConnectorModel.h>
#include "igtlioLogic.h"
#include "qIGTLIOConnectorPropertyWidget.h"
#include <QAction>

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

  igtlio::vtkIGTLIOConnectorPointer connector = Logic->GetConnector(row);
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

  QAction* addAction = new QAction("+", this);
  connect(addAction, SIGNAL(triggered()), this, SLOT(onAddConnectorButtonClicked()));

  QAction* removeAction = new QAction("-", this);
  connect(removeAction, SIGNAL(triggered()), this, SLOT(onRemoveConnectorButtonClicked()));

  QToolButton* addButton = new QToolButton(this);
  addButton->setDefaultAction(addAction);
  buttonLayout->addWidget(addButton);

  QToolButton* removeButton = new QToolButton(this);
  removeButton->setDefaultAction(removeAction);
  buttonLayout->addWidget(removeButton);

  buttonLayout->addStretch(1);



//  QPushButton* addConnectorButton = new QPushButton("+");
//  buttonLayout->addWidget(addConnectorButton);
//  QPushButton* removeConnectorButton = new QPushButton("--");
//  buttonLayout->addWidget(removeConnectorButton);
//  buttonLayout->addStretch();

//  //  Add(+) / Remove(-) Connector Buttons
//  connect(addConnectorButton, SIGNAL(clicked()), this,
//          SLOT(onAddConnectorButtonClicked()));
//  connect(removeConnectorButton, SIGNAL(clicked()), this,
//          SLOT(onRemoveConnectorButtonClicked()));
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::setLogic(igtlio::vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << igtlio::vtkIGTLIOLogic::ConnectionAddedEvent
          << igtlio::vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionsChanged(vtkObject*, unsigned long, void*, void*)));
    }

  this->Logic = logic;
  ConnectorModel->setLogic(logic);
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onConnectionsChanged(vtkObject* caller, unsigned long event, void * clientData,  void* connector)
{
  // remove removed connector from property widget
  if (event==igtlio::vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent && connector!=NULL)
    {
      igtlio::vtkIGTLIOConnector* c = static_cast<igtlio::vtkIGTLIOConnector*>(connector);
      if (ConnectorPropertyWidget->getMRMLIGTLConnectorNode().GetPointer() == c)
        ConnectorPropertyWidget->setMRMLIGTLConnectorNode(igtlio::vtkIGTLIOConnectorPointer());
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorListWidget::onAddConnectorButtonClicked()
{
  std::cout << "onAddConnectorButtonClicked " << Logic << std::endl;
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

