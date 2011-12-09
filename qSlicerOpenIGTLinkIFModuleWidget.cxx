// Qt includes
#include <QDebug>
#include <QStandardItemModel>
#include <QTreeView>

// SlicerQt includes
#include "qSlicerOpenIGTLinkIFModuleWidget.h"
#include "ui_qSlicerOpenIGTLinkIFModule.h"

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

  this->connect(d->addConnectorButton, SIGNAL(clicked()), this,
                SLOT(onAddConnectorButtonClicked()));
  this->connect(d->removeConnectorButton, SIGNAL(clicked()), this,
                SLOT(onRemoveConnectorButtonClicked()));
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);

  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
    return;
  d->connectorListView->setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onAddConnectorButtonClicked()
{
  Q_D(qSlicerOpenIGTLinkIFModuleWidget);

  //new QListViewItem(d->connectorList, "aaa", "bbbb", "cccc", "ddd");
  std::cerr << "Add Connector Button is clicked." << std::endl;

  vtkMRMLIGTLConnectorNode* connector = vtkMRMLIGTLConnectorNode::New();
  //int restrectDeviceName = (this->EnableAdvancedSettingButton->GetSelectedState())? 1:0;

  this->mrmlScene()->AddNode(connector);
  //connector->SetRestrictDeviceName(restrectDeviceName);
  //connector->Modified();

  //connector->Delete();

}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onRemoveConnectorButtonClicked()
{
  std::cerr << "Remove Connector Button is clicked." << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onServerSelected()
{
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModuleWidget::onClientSelected()
{
}

