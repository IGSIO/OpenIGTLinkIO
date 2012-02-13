/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLBase.cxx $
  Date:      $Date: 2009-10-05 17:19:02 -0400 (Mon, 05 Oct 2009) $
  Version:   $Revision: 10576 $

==========================================================================*/

// OpenIGTLink includes
#include <igtlMessageBase.h>

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"

#include "vtkSlicerOpenIGTLinkIFLogic.h"

// VTK includes
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLBase);
vtkCxxRevisionMacro(vtkIGTLToMRMLBase, "$Revision: 10576 $");

//---------------------------------------------------------------------------
class vtkIGTLToMRMLBasePrivate
{
public:
  vtkIGTLToMRMLBasePrivate();
  ~vtkIGTLToMRMLBasePrivate();

  void SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic* logic);
  vtkSlicerOpenIGTLinkIFLogic* GetOpenIGTLinkIFLogic();

protected:
  vtkSlicerOpenIGTLinkIFLogic* OpenIGTLinkIFLogic;
};

vtkIGTLToMRMLBasePrivate::vtkIGTLToMRMLBasePrivate()
{
  this->OpenIGTLinkIFLogic = NULL;
}

vtkIGTLToMRMLBasePrivate::~vtkIGTLToMRMLBasePrivate()
{
}

void vtkIGTLToMRMLBasePrivate::SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic* logic)
{
  this->OpenIGTLinkIFLogic = logic;
}


vtkSlicerOpenIGTLinkIFLogic* vtkIGTLToMRMLBasePrivate::GetOpenIGTLinkIFLogic()
{
  return this->OpenIGTLinkIFLogic;
}


//---------------------------------------------------------------------------
vtkIGTLToMRMLBase::vtkIGTLToMRMLBase()
{
  this->CheckCRC = 1;
  this->Private = new vtkIGTLToMRMLBasePrivate;
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLBase::~vtkIGTLToMRMLBase()
{
  if (this->Private)
    {
    delete this->Private;
    }
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLBase::SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic* logic)
{
  if (this->Private)
    {
    this->Private->SetOpenIGTLinkIFLogic(logic);
    }
}


//---------------------------------------------------------------------------
vtkSlicerOpenIGTLinkIFLogic* vtkIGTLToMRMLBase::GetOpenIGTLinkIFLogic()
{
  if (this->Private)
    {
    return this->Private->GetOpenIGTLinkIFLogic();
    }
  else
    {
    return NULL;
    }
}


