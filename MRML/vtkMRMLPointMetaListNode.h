/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLPointMetaListNode_h
#define __vtkMRMLPointMetaListNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkIGTLToMRMLBase.h"

#include <string>
#include <map>
#include <vector>
#include <set>

#include "vtkObject.h"


class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkMRMLPointMetaListNode : public vtkMRMLNode
{
 public:

  //----------------------------------------------------------------
  // Constants Definitions
  //----------------------------------------------------------------

  //BTX
  // Events
  enum {
    NewDeviceEvent        = 118949,
  };

  //ETX

  //BTX
  typedef struct {
    std::string   Name;        /* name / description (< 64 bytes)*/
    std::string   GroupName;   /* Can be "Labeled Point", "Landmark", Fiducial", ... */
    int     			RGBA[4];     /* Color in R/G/B/A */
    float   			Position[3]; /* Position */
    float   			Radius;      /* Radius of the point. Can be 0. */
    std::string   Owner;       /* Device name of the ower image */
  } PointMetaElement;
  //ETX

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLPointMetaListNode *New();
  vtkTypeMacro(vtkMRMLPointMetaListNode,vtkMRMLNode);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    { return "PointMetaList"; }

  // method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  //BTX
  // Description:
  // Get group names of point lists stored in this class instance
  void GetPointGroupNames(std::vector<std::string>& ids);

  // Description:
  // Get list of PointMetaElement with given group ID
  void GetPointGroup(std::string groupID, std::vector<PointMetaElement>& elements);
  //ETX

  // Description:
  // Add image meta element
  void AddPointMetaElement(PointMetaElement element);


  // Description:
  // Get point meta element. If the element does not eists,
  // DeviceName is set to "".
  void GetPointMetaElement(int index, PointMetaElement* element);

  // Description:
  // Clear point meta element list
  void ClearPointMetaList();

 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
  
  vtkMRMLPointMetaListNode();
  ~vtkMRMLPointMetaListNode();
  vtkMRMLPointMetaListNode(const vtkMRMLPointMetaListNode&);
  void operator=(const vtkMRMLPointMetaListNode&);

 public:
  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------


 private:
  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------

  //BTX
   std::map<std::string, std::vector<PointMetaElement>> PointGroups;
  //ETX

};

#endif

