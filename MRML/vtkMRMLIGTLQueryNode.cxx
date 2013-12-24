/*=auto=========================================================================

Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// OpenIGTLinkIF MRML includes
#include "vtkMRMLIGTLQueryNode.h"

// OpenIGTLink includes
#include <igtlOSUtil.h>
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>
#include <igtl_header.h>  // to define maximum length of message name

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkObjectFactory.h>

// STD includes
#include <string>
#include <iostream>
#include <sstream>
#include <map>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLIGTLQueryNode);

//----------------------------------------------------------------------------
vtkMRMLIGTLQueryNode::vtkMRMLIGTLQueryNode()
{
  this->QueryStatus = 0;
  this->QueryType   = 0;

  this->ConnectorNodeID = "";

  this->TimeStamp = 0.0;
  this->TimeOut   = 0.0;

  this->NoNameQuery = 0;

}

//----------------------------------------------------------------------------
vtkMRMLIGTLQueryNode::~vtkMRMLIGTLQueryNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLQueryNode::WriteXML(ostream& of, int nIndent)
{
  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

}


//----------------------------------------------------------------------------
void vtkMRMLIGTLQueryNode::ReadXMLAttributes(const char** atts)
{

  vtkMRMLNode::ReadXMLAttributes(atts);

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLIGTLQueryNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  //vtkMRMLIGTLQueryNode *node = (vtkMRMLIGTLQueryNode *) anode;

}


//----------------------------------------------------------------------------
void vtkMRMLIGTLQueryNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{

  Superclass::ProcessMRMLEvents(caller, event, callData);

}


//----------------------------------------------------------------------------
void vtkMRMLIGTLQueryNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLIGTLQueryNode::SetIGTLName(const char* name)
{
  char buf[IGTL_HEADER_DEVSIZE+1];
  buf[IGTL_HEADER_DEVSIZE] = '\0';
  strncpy(buf, name, IGTL_HEADER_DEVSIZE);
  this->IGTLName = buf;

}


//----------------------------------------------------------------------------
const char* vtkMRMLIGTLQueryNode::GetErrorString()
{
  return "";

}
