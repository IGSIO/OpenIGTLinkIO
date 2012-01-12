
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

  vtkMRMLIGTLConnectorNode * ConnectorNode;
  int Direction;
};

//------------------------------------------------------------------------------
qSlicerIGTLIONodeSelectorWidgetPrivate::qSlicerIGTLIONodeSelectorWidgetPrivate(qSlicerIGTLIONodeSelectorWidget& object)
  : q_ptr(&object)
{
  //this->CurrentNode = NULL;
  this->ConnectorNode = 0;
  this->Direction = qSlicerIGTLIONodeSelectorWidget::UNDEFINED;
}

//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidgetPrivate::init()
{
  Q_Q(qSlicerIGTLIONodeSelectorWidget);
  this->setupUi(q);


  //QObject::connect(this->NodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
  //this, SLOT(setAddingNode(vtkMRMLNode*)));
  //QObject::connect(this, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
  //this->NodeSelector, SLOT(setMRML(vtkMRMLNode*)));

  QObject::connect(this->AddNodeButton, SIGNAL(clicked()),
                   q, SLOT(onAddNodeButtonClicked()));
  QObject::connect(this->RemoveNodeButton, SIGNAL(clicked()),
                   q, SLOT(onRemoveNodeButtonClicked()));

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
    //d->NodeSelector->setEnabled(true);
    //d->NodeSelector->setCurrentNode(node);
    return;
    }

  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    // Connector node specified
    d->AddNodeButton->setEnabled(false);
    d->RemoveNodeButton->setEnabled(false);
    //d->NodeSelector->setEnabled(false);
    //d->NodeSelector->setCurrentNode(node);
    return;
    }
  else
    {
    // Standard data node specified
    d->AddNodeButton->setEnabled(true);
    d->RemoveNodeButton->setEnabled(true);
    //d->NodeSelector->setEnabled(true);
    //d->NodeSelector->setCurrentNode(node);
    }
}


//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::updateIGTLConnectorNode(vtkMRMLIGTLConnectorNode* node, int dir)
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);

  d->ConnectorNode = node;
  d->Direction = dir;
  
}


//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::onAddNodeButtonClicked()
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);

  vtkMRMLNode* node = d->NodeSelector->currentNode();
  if (node == 0)
    {
    return;
    }

  if (d->ConnectorNode)
    {
    if (d->Direction == 0)
      {
      d->ConnectorNode->RegisterIncomingMRMLNode(node);
      }
    else if (d->Direction == 1)
      {
      d->ConnectorNode->RegisterOutgoingMRMLNode(node);
      }
    }
  
  //emit addNode(node);

}


//------------------------------------------------------------------------------
void qSlicerIGTLIONodeSelectorWidget::onRemoveNodeButtonClicked()
{
  Q_D(qSlicerIGTLIONodeSelectorWidget);

  vtkMRMLNode* node = d->NodeSelector->currentNode();
  if (node == 0)
    {
    return;
    }
  
  //emit removeNode(node);

}


