/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL:  $
  Date:      $Date:  $
  Version:   $Revision: $

==========================================================================*/

// Qt includes
#include <QtPlugin>

// OpenIGTLinkIF MRML includes
#include "qSlicerOpenIGTLinkIFModule.h"
#include "qSlicerOpenIGTLinkIFModuleWidget.h"

// OpenIGTLinkIF Logic includes
#include <vtkSlicerOpenIGTLinkIFLogic.h>

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
  return "The OpenIGTLink IF module manages communications between 3D Slicer"
         " and other OpenIGTLink-compliant software through the network.";
}

//-----------------------------------------------------------------------------
QString qSlicerOpenIGTLinkIFModule::acknowledgementText()const
{
  return "This module was developed by Junichi Tokuda (Brigham and Women's Hospital), "
         "Jean-Christophe Fillion-Robin (Kitware, Inc.) and OpenIGTLink community."
         "The research was funded by NIH (R01CA111288, P41RR019703, P41EB015898, "
         "P01CA067165, U54EB005149) and NEDO, Japan.";
}

//-----------------------------------------------------------------------------
QIcon qSlicerOpenIGTLinkIFModule::icon()const
{
  return QIcon(":/Icons/OpenIGTLinkIF.png");
}


//-----------------------------------------------------------------------------
QStringList qSlicerOpenIGTLinkIFModule::categories() const
{
  return QStringList() << "" << "IGT";
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
