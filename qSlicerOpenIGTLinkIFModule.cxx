
// Qt includes
#include <QtPlugin>

// OpenIGTLinkIF Logic includes
#include <vtkSlicerOpenIGTLinkIFLogic.h>

// OpenIGTLinkIF includes
#include "qSlicerOpenIGTLinkIFModule.h"
#include "qSlicerOpenIGTLinkIFModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerOpenIGTLinkIFModule, qSlicerOpenIGTLinkIFModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qSlicerOpenIGTLinkIFModulePrivate
{
public:
  qSlicerOpenIGTLinkIFModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModulePrivate::qSlicerOpenIGTLinkIFModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkIFModule methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModule::qSlicerOpenIGTLinkIFModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerOpenIGTLinkIFModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkIFModule::~qSlicerOpenIGTLinkIFModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerOpenIGTLinkIFModule::helpText()const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerOpenIGTLinkIFModule::acknowledgementText()const
{
  return "This work was supported by ...";
}

//-----------------------------------------------------------------------------
QIcon qSlicerOpenIGTLinkIFModule::icon()const
{
  return QIcon(":/Icons/OpenIGTLinkIF.png");
}

//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkIFModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerOpenIGTLinkIFModule::createWidgetRepresentation()
{
  return new qSlicerOpenIGTLinkIFModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerOpenIGTLinkIFModule::createLogic()
{
  return vtkSlicerOpenIGTLinkIFLogic::New();
}
