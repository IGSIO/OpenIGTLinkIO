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

  connect(d->AddConnectorButton, SIGNAL(clicked()), this,
          SLOT(onAddConnectorButtonClicked()));
  connect(d->RemoveConnectorButton, SIGNAL(clicked()), this,
          SLOT(onRemoveConnectorButtonClicked()));
  connect(d->ConnectorListView, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          d->ConnectorPropertyWidget, SLOT(setMRMLIGTLConnectorNode(vtkMRMLNode*)));
  d->ConnectorPropertyWidget->setMRMLIGTLConnectorNode(static_cast<vtkMRMLNode*>(0));

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
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onAddConnectorButtonClicked()
{
  qMRMLNodeFactory::createNode(this->mrmlScene(), "vtkMRMLIGTLConnectorNode");

  //int restrectDeviceName = (this->EnableAdvancedSettingButton->GetSelectedState())? 1:0;
  //connector->SetRestrictDeviceName(restrectDeviceName);
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
void qSlicerOpenIGTLinkIFModuleWidget::onServerSelected()
{
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onClientSelected()
{
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::importDataAndEvents()
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);
  d->logic()->ImportEvents();
  d->logic()->ImportFromCircularBuffers();
}
