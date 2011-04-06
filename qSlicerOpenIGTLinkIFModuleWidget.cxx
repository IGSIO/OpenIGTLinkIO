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
}

