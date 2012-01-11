
// Qt includes
#include <QButtonGroup>

// OpenIGTLinkIF GUI includes
#include "qSlicerIGTLIONodeSelectorWidget.h"
#include "ui_qSlicerIGTLIONodeSelectorWidget.h"

// OpenIGTLinkIF MRML includes
#include "vtkMRMLIGTLConnectorNode.h"

//------------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerIGTLIONodeSelectorWidgetPrivate : public Ui_qSlicerIGTLIONodeSelectorWidget
{
  Q_DECLARE_PUBLIC(qSlicerIGTLIONodeSelectorWidget);
protected:
  qSlicerIGTLIONodeSelectorWidget* const q_ptr;
public:
  qSlicerIGTLIONodeSelectorWidgetPrivate(qSlicerIGTLIONodeSelectorWidget& object);
  void init();

  //vtkMRMLIGTLConnectorNode * IGTLConnectorNode;

  QButtonGroup ConnectorTypeButtonGroup;
};

//------------------------------------------------------------------------------
qSlicerIGTLIONodeSelectorWidgetPrivate::qSlicerIGTLIONodeSelectorWidgetPrivate(qSlicerIGTLIONodeSelectorWidget& object)
  : q_ptr(&object)
{
  //this->IGTLConnectorNode = 0;
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidgetPrivate::init()
{
  Q_Q(qSlicerIGTLIONodeSelectorWidget);
  this->setupUi(q);
  //QObject::connect(this->ConnectorNameEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(this->ConnectorStateCheckBox, SIGNAL(toggled(bool)),
  //                 q, SLOT(startCurrentIGTLConnector(bool)));
  //QObject::connect(this->ConnectorHostNameEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(this->ConnectorPortEdit, SIGNAL(editingFinished()),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //QObject::connect(&this->ConnectorTypeButtonGroup, SIGNAL(buttonClicked(int)),
  //                 q, SLOT(updateIGTLConnectorNode()));
  //this->ConnectorNotDefinedRadioButton->setVisible(false);
  //this->ConnectorTypeButtonGroup.addButton(this->ConnectorNotDefinedRadioButton, vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);
  //this->ConnectorTypeButtonGroup.addButton(this->ConnectorServerRadioButton, vtkMRMLIGTLConnectorNode::TYPE_SERVER);
  //this->ConnectorTypeButtonGroup.addButton(this->ConnectorClientRadioButton, vtkMRMLIGTLConnectorNode::TYPE_CLIENT);

}

//------------------------------------------------------------------------------
qSlicerIGTLIONodeSelectorWidget::qSlicerIGTLIONodeSelectorWidget(QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerIGTLIONodeSelectorWidgetPrivate(*this))
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);
  d->init();
}

//------------------------------------------------------------------------------
qSlicerIGTLIONodeSelectorWidget::~qSlicerIGTLIONodeSelectorWidget()
{
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);
  if (d->NodeSelector)
    {
    d->NodeSelector->setMRMLScene(scene);
    }
}


//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::setCurrentNode(vtkMRMLNode* node)
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);
  
  if (node == NULL)
    {
    // No node specified
    d->AddNodeButton->setEnabled(true);
    d->RemoveNodeButton->setEnabled(false);
    d->NodeSelector->setEnabled(true);
    d->NodeSelector->setCurrentNode(node);
    return;
    }

  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    // Connector node specified
    d->AddNodeButton->setEnabled(false);
    d->RemoveNodeButton->setEnabled(false);
    d->NodeSelector->setEnabled(false);
    d->NodeSelector->setCurrentNode(node);
    return;
    }
  else
    {
    // Standard data node specified
    d->AddNodeButton->setEnabled(true);
    d->RemoveNodeButton->setEnabled(true);
    d->NodeSelector->setEnabled(true);
    d->NodeSelector->setCurrentNode(node);
    }
}


//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::setMRMLIGTLConnectorNode(vtkMRMLIGTLConnectorNode * connectorNode)
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);

  //qvtkReconnect(d->IGTLConnectorNode, connectorNode, vtkCommand::ModifiedEvent,
  //              this, SLOT(onMRMLNodeModified()));

  foreach(int evendId, QList<int>()
          << vtkMRMLIGTLConnectorNode::ActivatedEvent
          << vtkMRMLIGTLConnectorNode::ConnectedEvent
          << vtkMRMLIGTLConnectorNode::DisconnectedEvent
          << vtkMRMLIGTLConnectorNode::DeactivatedEvent)
    {
    //qvtkReconnect(d->IGTLConnectorNode, connectorNode, evendId,
    //              this, SLOT(onMRMLNodeModified()));
    }

  //d->IGTLConnectorNode = connectorNode;

  this->onMRMLNodeModified();
  this->setEnabled(connectorNode != 0);
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::setMRMLIGTLConnectorNode(vtkMRMLNode* node)
{
  this->setMRMLIGTLConnectorNode(vtkMRMLIGTLConnectorNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::onMRMLNodeModified()
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);
  //if (!d->IGTLConnectorNode)
  //  {
  //  return;
  //  }
  //d->ConnectorNameEdit->setText(d->IGTLConnectorNode->GetName());
  //d->ConnectorHostNameEdit->setText(d->IGTLConnectorNode->GetServerHostname());
  //d->ConnectorPortEdit->setText(QString("%1").arg(d->IGTLConnectorNode->GetServerPort()));
  //int type = d->IGTLConnectorNode->GetType();
  //d->ConnectorNotDefinedRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);
  //d->ConnectorServerRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_SERVER);
  //d->ConnectorClientRadioButton->setChecked(type == vtkMRMLIGTLConnectorNode::TYPE_CLIENT);

  //setStateEnabled(d, type != vtkMRMLIGTLConnectorNode::TYPE_NOT_DEFINED);

  //bool deactivated = d->IGTLConnectorNode->GetState() == vtkMRMLIGTLConnectorNode::STATE_OFF;
  //if (deactivated)
  //  {
  //  }
  //else
  //  {
  //  }
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::startCurrentIGTLConnector(bool value)
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);
  //Q_ASSERT(d->IGTLConnectorNode);
  //if (value)
  //  {
  //  d->IGTLConnectorNode->Start();
  //  }
  //else
  //  {
  //  d->IGTLConnectorNode->Stop();
  //  }
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::updateIGTLConnectorNode()
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);

  //d->IGTLConnectorNode->DisableModifiedEventOn();
  //
  //d->IGTLConnectorNode->SetName(d->ConnectorNameEdit->text().toLatin1());
  //d->IGTLConnectorNode->SetType(d->ConnectorTypeButtonGroup.checkedId());
  //d->IGTLConnectorNode->SetServerHostname(d->ConnectorHostNameEdit->text().toStdString());
  //d->IGTLConnectorNode->SetServerPort(d->ConnectorPortEdit->text().toInt());
  //
  //d->IGTLConnectorNode->DisableModifiedEventOff();
  //d->IGTLConnectorNode->InvokePendingModifiedEvent();
}
