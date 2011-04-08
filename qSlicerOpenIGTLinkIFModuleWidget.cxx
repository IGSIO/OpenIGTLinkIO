// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerOpenIGTLinkIFModuleWidget.h"
#include "ui_qSlicerOpenIGTLinkIFModule.h"

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
void qSlicerOpenIGTLinkIFModuleWidget::onAddConnectorButtonClicked()
{
  std::cerr << "Add Connector Button is clicked." << std::endl;
}


void qSlicerOpenIGTLinkIFModuleWidget::onRemoveConnectorButtonClicked()
{
  std::cerr << "Remove Connector Button is clicked." << std::endl;
}


void qSlicerOpenIGTLinkIFModuleWidget::onServerSelected()
{
}


void qSlicerOpenIGTLinkIFModuleWidget::onClientSelected()
{
}

