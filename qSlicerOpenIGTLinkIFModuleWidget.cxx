// Qt includes
#include <QDebug>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// SlicerQt includes
#include "qSlicerOpenIGTLinkIFModuleWidget.h"
#include "ui_qSlicerOpenIGTLinkIFModule.h"

// qMRMLWidgets includes
#include <qMRMLNodeFactory.h>

// OpenIGTLinkIF Logic includes
#include "vtkSlicerOpenIGTLinkIFLogic.h"

// OpenIGTLinkIF MRML includes
#include "vtkMRMLIGTLConnectorNode.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerOpenIGTLinkIFModuleWidgetPrivate: public Ui_qSlicerOpenIGTLinkIFModule
{
  Q_DECLARE_PUBLIC(qSlicerOpenIGTLinkIFModuleWidget);
protected:
  qSlicerOpenIGTLinkIFModuleWidget* const q_ptr;
public:
  qSlicerOpenIGTLinkIFModuleWidgetPrivate(qSlicerOpenIGTLinkIFModuleWidget& object);

  vtkSlicerOpenIGTLinkIFLogic * logic();

  QTimer ImportDataAndEventsTimer;
};

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModuleWidgetPrivate::qSlicerOpenIGTLinkIFModuleWidgetPrivate(qSlicerOpenIGTLinkIFModuleWidget& object)
 : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerOpenIGTLinkIFLogic * qSlicerOpenIGTLinkIFModuleWidgetPrivate::logic()
{
  Q_Q(qSlicerOpenIGTLinkIFModuleWidget);
  return vtkSlicerOpenIGTLinkIFLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModuleWidget::qSlicerOpenIGTLinkIFModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerOpenIGTLinkIFModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModuleWidget::~qSlicerOpenIGTLinkIFModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::setup()
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  
  // --------------------------------------------------
  // Connectors section
  //  Connector List View
  connect(d->ConnectorListView, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          d->ConnectorPropertyWidget, SLOT(setMRMLIGTLConnectorNode(vtkMRMLNode*)));
  d->ConnectorPropertyWidget->setMRMLIGTLConnectorNode(static_cast<vtkMRMLNode*>(0));

  //  Add(+) / Remove(-) Connector Buttons
  connect(d->AddConnectorButton, SIGNAL(clicked()), this,
          SLOT(onAddConnectorButtonClicked()));
  connect(d->RemoveConnectorButton, SIGNAL(clicked()), this,
          SLOT(onRemoveConnectorButtonClicked()));

  // --------------------------------------------------
  //  I/O Configuration Section
  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)),
          d->IGTIONodeSelectorWidget, SLOT(setMRMLScene(vtkMRMLScene*)));
  connect(d->IOTreeView, SIGNAL(ioTreeViewUpdated(int,vtkMRMLIGTLConnectorNode*,int)),
          d->IGTIONodeSelectorWidget, SLOT(updateEnabledStatus(int,vtkMRMLIGTLConnectorNode*,int)));

  // --------------------------------------------------
  //  Visualization Section
  connect(d->EnableLocatorDriverCheckBox, SIGNAL(toggled(bool)), this,
          SLOT(setLocatorDriverVisible(bool)));
  this->setLocatorDriverVisible(d->EnableLocatorDriverCheckBox->isChecked());

  // TODO We should probably listen for the logic and implement a onLogicModified() slot
  //      Doing so we would be able to update the UI if the locator is externally enabled.

  connect(&d->ImportDataAndEventsTimer, SIGNAL(timeout()),
          this, SLOT(importDataAndEvents()));
  d->ImportDataAndEventsTimer.start(5);


}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);

  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
    {
    return;
    }
  d->ConnectorListView->setMRMLScene(scene);
  d->IOTreeView->setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onAddConnectorButtonClicked()
{
  qMRMLNodeFactory::createNode(this->mrmlScene(), "vtkMRMLIGTLConnectorNode");
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onRemoveConnectorButtonClicked()
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);
  vtkMRMLNode * node = d->ConnectorListView->currentNode();
  if (!node)
    {
    return;
    }
  vtkMRMLIGTLConnectorNode * connectorNode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  Q_ASSERT(connectorNode);
  connectorNode->Stop();
  this->mrmlScene()->RemoveNode(connectorNode);
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::setLocatorDriverVisible(bool visible)
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);
  d->logic()->EnableLocatorDriver(visible);
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::importDataAndEvents()
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);
  d->logic()->ImportEvents();
  d->logic()->ImportFromCircularBuffers();
}
