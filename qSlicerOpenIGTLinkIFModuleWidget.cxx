// Qt includes
#include <QDebug>
#include <QStandardItemModel>
#include <QTreeView>

// SlicerQt includes
#include "qSlicerOpenIGTLinkIFModuleWidget.h"
#include "ui_qSlicerOpenIGTLinkIFModule.h"

// qMRMLWidgets includes
#include <qMRMLNodeFactory.h>

// OpenIGTLinkIF MRML includes
#include "vtkMRMLIGTLConnectorNode.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerOpenIGTLinkIFModuleWidgetPrivate: public Ui_qSlicerOpenIGTLinkIFModule
{
public:
  qSlicerOpenIGTLinkIFModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModuleWidgetPrivate::qSlicerOpenIGTLinkIFModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModuleWidget::qSlicerOpenIGTLinkIFModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerOpenIGTLinkIFModuleWidgetPrivate )
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

  this->connect(d->AddConnectorButton, SIGNAL(clicked()), this,
                SLOT(onAddConnectorButtonClicked()));
  this->connect(d->RemoveConnectorButton, SIGNAL(clicked()), this,
                SLOT(onRemoveConnectorButtonClicked()));
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

