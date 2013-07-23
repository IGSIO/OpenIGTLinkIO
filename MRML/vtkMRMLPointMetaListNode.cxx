/*=auto=========================================================================

Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"

#include "vtkMRMLPointMetaListNode.h"
#include "vtkMRMLScene.h"

#include "igtlOSUtil.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"

typedef std::map<std::string, std::vector<vtkMRMLPointMetaListNode::PointMetaElement>> PointGroupsType;

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLPointMetaListNode);

//----------------------------------------------------------------------------
vtkMRMLPointMetaListNode::vtkMRMLPointMetaListNode()
{
  this->ClearPointMetaList();
}

//----------------------------------------------------------------------------
vtkMRMLPointMetaListNode::~vtkMRMLPointMetaListNode()
{
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::WriteXML(ostream& of, int nIndent)
{
  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPointMetaListNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::GetPointGroupNames(std::vector<std::string>& names)
{
  PointGroupsType::iterator iter = this->PointGroups.begin();
  for (; iter != this->PointGroups.end(); iter++)
    names.push_back(iter->first);  
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::
AddPointMetaElement(PointMetaElement element)
{
  this->PointGroups[element.GroupName].push_back(element);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::
GetPointGroup(std::string groupID, std::vector<PointMetaElement>& elements)
{
  std::vector<PointMetaElement> ptgroup = this->PointGroups[groupID];
  std::vector<PointMetaElement>::iterator ptiter = ptgroup.begin();
  for (; ptiter != ptgroup.end(); ptiter++)
    elements.push_back(*ptiter);
}


//----------------------------------------------------------------------------
void vtkMRMLPointMetaListNode::ClearPointMetaList()
{
  PointGroupsType::iterator iter =
      this->PointGroups.begin();
  for (; iter != this->PointGroups.end(); iter++)
    iter->second.clear();
}
