/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLTrajectory.h"

// OpenIGTLink includes
#include <igtlStatusMessage.h>
#include <igtlWin32Header.h>

// MRML includes
#include <vtkMRMLLinearTransformNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLTrajectory);

//---------------------------------------------------------------------------
vtkIGTLToMRMLTrajectory::vtkIGTLToMRMLTrajectory()
{
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLTrajectory::~vtkIGTLToMRMLTrajectory()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLTrajectory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLTrajectory::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLAnnotationHierarchyNode* hierarchyNode;

  hierarchyNode = vtkMRMLAnnotationHierarchyNode::New();
  hierarchyNode->SetName(name);
  hierarchyNode->SetDescription("Received by OpenIGTLink");
  hierarchyNode->SetParentNodeID(NULL);
  hierarchyNode->HideFromEditorsOff();

  vtkMRMLNode* n = scene->AddNode(hierarchyNode);
  hierarchyNode->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLTrajectory::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLHierarchyNode::ChildNodeAddedEvent);
  events->InsertNextValue(vtkMRMLHierarchyNode::ChildNodeRemovedEvent);

  return events;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLTrajectory::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkIGTLToMRMLBase::IGTLToMRML(buffer, node);

  // Create a message buffer to receive transform data
  igtl::TrajectoryMessage::Pointer trajMsg;
  trajMsg = igtl::TrajectoryMessage::New();
  trajMsg->Copy(buffer);  // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = trajMsg->Unpack(this->CheckCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  if (node == NULL)
    {
    return 0;
    }

  vtkMRMLAnnotationHierarchyNode* hierarchyNode =
    vtkMRMLAnnotationHierarchyNode::SafeDownCast(node);
  if (!hierarchyNode)
    {
    return 0;
    }

  // Update hierarchy node
  int nOfTrajectories = trajMsg->GetNumberOfTrajectoryElement();
  for (int i = 0; i < nOfTrajectories; ++i)
    {
    igtl::TrajectoryElement::Pointer tElemt;
    trajMsg->GetTrajectoryElement(i, tElemt);

    if (tElemt.IsNotNull())
      {
      this->CrossCheckTrajectoryName(tElemt, trajMsg);
      this->AddTrajectoryElement(tElemt, hierarchyNode);
      }
    }

  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLTrajectory::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{

  if (mrmlNode && (event == vtkMRMLHierarchyNode::ChildNodeAddedEvent ||
                   event == vtkMRMLHierarchyNode::ChildNodeRemovedEvent))
    {
    vtkMRMLAnnotationHierarchyNode* hierarchyNode =
      vtkMRMLAnnotationHierarchyNode::SafeDownCast(mrmlNode);

    if (!hierarchyNode)
      {
      return 0;
      }

    if (this->OutTrajectoryMsg.IsNull())
      {
      this->OutTrajectoryMsg = igtl::TrajectoryMessage::New();
      }

    this->OutTrajectoryMsg->SetDeviceName(hierarchyNode->GetName());

    // Add all trajectory elements from hierarchy node
    int nOfAnnotations = hierarchyNode->GetNumberOfChildrenNodes();
    for (int i = 0; i < nOfAnnotations; ++i)
      {
      vtkMRMLHierarchyNode* hierarchyItem = hierarchyNode->GetNthChildNode(i);
      if (hierarchyItem)
        {
        vtkMRMLNode* tmpItem = hierarchyItem->GetAssociatedNode();
        if (tmpItem)
          {
          igtl::TrajectoryElement::Pointer trajectory;
          trajectory = igtl::TrajectoryElement::New();

          if (this->PrepareTrajectoryElement(trajectory, tmpItem))
            {
            this->OutTrajectoryMsg->AddTrajectoryElement(trajectory);
            }
          }
        }
      }

    this->OutTrajectoryMsg->Pack();

    *size = this->OutTrajectoryMsg->GetPackSize();
    *igtlMsg = (void*)this->OutTrajectoryMsg->GetPackPointer();

    // BUG: The parameter is not necessary for this method
    igtl::TrajectoryElement::Pointer dummy;
    this->OutTrajectoryMsg->ClearTrajectoryElement(dummy);

    return 1;
    }

  return 0;
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLTrajectory::AddTrajectoryElement(igtl::TrajectoryElement::Pointer& tElemt,
                                                   vtkMRMLAnnotationHierarchyNode* hierarchyNode)
{
  // TODO: After Nicole implemented rulers on Markups (Winter Project Week 2014)
  // replace annotations with markups

  if (tElemt.IsNull() || !hierarchyNode)
    {
    return;
    }

  vtkMRMLScene* scene = hierarchyNode->GetScene();
  if (!scene)
    {
    return;
    }

  // Check if element is already in hierarchy node
  char* annotationID = NULL;
  int nOfAnnotations = hierarchyNode->GetNumberOfChildrenNodes();
  for (int i = 0; i < nOfAnnotations; ++i)
    {
    vtkMRMLHierarchyNode* hierarchyItem = hierarchyNode->GetNthChildNode(i);
    if (hierarchyItem)
      {
      vtkMRMLNode* tmpItem = hierarchyItem->GetAssociatedNode();
      if (tmpItem)
        {
        const char* itemName = tmpItem->GetName();
        const char* elemtName = tElemt->GetName();
        if (itemName && elemtName)
          {
          if (strcmp(itemName, elemtName) == 0)
            {
            annotationID = tmpItem->GetID();
            break;
            }
          }
        }
      }
    }

  // If not, add an annotation based on trajectory type
  if (!annotationID)
    {
    if (tElemt->GetType() == igtl::TrajectoryElement::TYPE_ENTRY_TARGET)
      {
      vtkSmartPointer<vtkMRMLAnnotationRulerNode> ruler =
        vtkSmartPointer<vtkMRMLAnnotationRulerNode>::New();
      ruler->SetPosition1(0.0, 0.0, 0.0);
      ruler->SetPosition2(0.0, 0.0, 0.0);
      ruler->Initialize(scene);
      ruler->SetLocked(1);
      ruler->SelectedOn();
      annotationID = ruler->GetID();

      vtkSmartPointer<vtkMRMLAnnotationHierarchyNode> associatedHierarchyNode =
        vtkSmartPointer<vtkMRMLAnnotationHierarchyNode>::New();
      associatedHierarchyNode->SetAssociatedNodeID(annotationID);
      associatedHierarchyNode->SetParentNodeID(hierarchyNode->GetID());
      scene->AddNode(associatedHierarchyNode.GetPointer());
      }
    else
      {
      vtkSmartPointer<vtkMRMLAnnotationFiducialNode> fiducial =
        vtkSmartPointer<vtkMRMLAnnotationFiducialNode>::New();
      fiducial->SetFiducialCoordinates(0.0, 0.0, 0.0);
      fiducial->Initialize(scene);
      fiducial->SetLocked(1);
      fiducial->SelectedOn();
      annotationID = fiducial->GetID();

      vtkSmartPointer<vtkMRMLAnnotationHierarchyNode> associatedHierarchyNode =
        vtkSmartPointer<vtkMRMLAnnotationHierarchyNode>::New();
      associatedHierarchyNode->SetAssociatedNodeID(annotationID);
      associatedHierarchyNode->SetParentNodeID(hierarchyNode->GetID());
      scene->AddNode(associatedHierarchyNode.GetPointer());
      }
    }

  // Get annotation node from scene
  vtkMRMLNode* annotationNode = NULL;
  if (annotationID)
    {
    annotationNode = scene->GetNodeByID(annotationID);
    }

  // If node exists, update it's values
  if (annotationNode)
    {
    if (tElemt->GetType() == igtl::TrajectoryElement::TYPE_ENTRY_TARGET)
      {
      this->UpdateRulerAnnotation(tElemt, annotationNode);
      }
    else
      {
      this->UpdateFiducialAnnotation(tElemt, annotationNode);
      }
    }
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLTrajectory::UpdateRulerAnnotation(igtl::TrajectoryElement::Pointer& tElemt,
                                                    vtkMRMLNode* node)
{
  vtkMRMLAnnotationRulerNode* ruler = NULL;
  if (node->IsA("vtkMRMLAnnotationRulerNode"))
    {
    ruler = vtkMRMLAnnotationRulerNode::SafeDownCast(node);
    }

  if (!ruler)
    {
    return;
    }

  // Update name (if not empty, otherwise keep ruler's name)
  if (strcmp(tElemt->GetName(), "") != 0)
    {
    ruler->SetName(tElemt->GetName());
    }

  // Update positions
  float entryPos[3], targetPos[3];
  tElemt->GetEntryPosition(entryPos);
  tElemt->GetTargetPosition(targetPos);
  ruler->SetPosition1(entryPos[0], entryPos[1], entryPos[2]);
  ruler->SetPosition2(targetPos[0], targetPos[1], targetPos[2]);

  // Update color
  unsigned char rgba[4];
  tElemt->GetRGBA(rgba);
  vtkMRMLAnnotationLineDisplayNode* display = ruler->GetAnnotationLineDisplayNode();
  if (display)
    {
    display->SetColor(rgba[0]/255, rgba[1]/255, rgba[2]/255);
    display->SetOpacity(rgba[3]/255);
    }
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLTrajectory::UpdateFiducialAnnotation(igtl::TrajectoryElement::Pointer& tElemt,
                                                       vtkMRMLNode* node)
{
  vtkMRMLAnnotationFiducialNode* fiducial = NULL;
  if (node->IsA("vtkMRMLAnnotationFiducialNode"))
    {
    fiducial = vtkMRMLAnnotationFiducialNode::SafeDownCast(node);
    }

  if (!fiducial)
    {
    return;
    }

  // Update name (if not empty, otherwise keep fiducial's name)
  if (strcmp(tElemt->GetName(), "") != 0)
    {
    fiducial->SetName(tElemt->GetName());
    }

  // Update position
  float fiducialPos[3];
  std::stringstream typeAttribute;
  if (tElemt->GetType() == igtl::TrajectoryElement::TYPE_ENTRY_ONLY)
    {
    tElemt->GetEntryPosition(fiducialPos);
    typeAttribute << "EntryPoint";
    }
  else
    {
    tElemt->GetTargetPosition(fiducialPos);
    typeAttribute << "TargetPoint";
    }
  fiducial->SetFiducialCoordinates(fiducialPos[0], fiducialPos[1], fiducialPos[2]);

  // Update color and radius
  unsigned char rgba[4];
  tElemt->GetRGBA(rgba);
  vtkMRMLAnnotationPointDisplayNode* display = fiducial->GetAnnotationPointDisplayNode();
  if (display)
    {
    display->SetColor(rgba[0]/255, rgba[1]/255, rgba[2]/255);
    display->SetOpacity(rgba[3]/255);
    }

  // Update Attribute
  if (!fiducial->GetAttribute("OpenIGTLinkIF.TrajectoryType"))
    {
    fiducial->SetAttribute("OpenIGTLinkIF.TrajectoryType", typeAttribute.str().c_str());
    }
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLTrajectory::PrepareTrajectoryElement(igtl::TrajectoryElement::Pointer& tElemt,
                                                       vtkMRMLNode* node)
{
  if (tElemt.IsNull() || !node)
    {
    return 0;
    }

  // Ruler of Fiducial ?
  vtkMRMLAnnotationRulerNode* ruler
    = vtkMRMLAnnotationRulerNode::SafeDownCast(node);
  vtkMRMLAnnotationFiducialNode* fiducial
    = vtkMRMLAnnotationFiducialNode::SafeDownCast(node);

  // Gather necessary information
  vtkMRMLAnnotationDisplayNode* displayNode = NULL;
  const char*   name = node->GetName();
  double*       rgba;
  double        opacity;
  double        entryPos[3] = {0.0, 0.0, 0.0};
  double        targetPos[3] = {0.0, 0.0, 0.0};
  unsigned char trajectoryType = igtl::TrajectoryElement::TYPE_ENTRY_TARGET;

  if (ruler)
    {
    displayNode = ruler->GetAnnotationLineDisplayNode();
    ruler->GetPosition1(entryPos);
    ruler->GetPosition2(targetPos);
    }
  else if (fiducial)
    {
    displayNode = fiducial->GetAnnotationPointDisplayNode();
    const char* trajectoryAttribute = fiducial->GetAttribute("OpenIGTLinkIF.TrajectoryType");
    if (trajectoryAttribute && strcmp(trajectoryAttribute, "EntryPoint") == 0)
      {
      trajectoryType = igtl::TrajectoryElement::TYPE_ENTRY_ONLY;
      fiducial->GetFiducialCoordinates(entryPos);
      }
    else
      {
      trajectoryType = igtl::TrajectoryElement::TYPE_TARGET_ONLY;
      fiducial->GetFiducialCoordinates(targetPos);
      }
    }
  else
    {
    return 0;
    }

  // Get display information
  if (displayNode)
    {
    rgba = displayNode->GetColor();
    opacity = displayNode->GetOpacity();
    }

  // Set information to elements
  tElemt->SetName(name);
  tElemt->SetType(trajectoryType);
  tElemt->SetRGBA(rgba[0]*255, rgba[1]*255, rgba[2]*255, opacity*255);
  tElemt->SetEntryPosition(entryPos[0], entryPos[1], entryPos[2]);
  tElemt->SetTargetPosition(targetPos[0], targetPos[1], targetPos[2]);

  return 1;
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLTrajectory::CrossCheckTrajectoryName(igtl::TrajectoryElement::Pointer& tElemt,
						       igtl::TrajectoryMessage::Pointer& trajMsg)
{
  if (tElemt.IsNull() || trajMsg.IsNull())
    {
    return;
    }

  const char* elementName = tElemt->GetName();
  if (!elementName)
    {
    return;
    }

  igtl::TrajectoryElement::Pointer otherElemt;
  if (strcmp(elementName, "") == 0)
    {
    // If name is empty, it will be name with annotation name later
    // Should be ignored here
    return;
    }

  int nOfTrajectories = trajMsg->GetNumberOfTrajectoryElement();
  for (int j = 0; j < nOfTrajectories; ++j)
    {
    if (tElemt == otherElemt)
      {
      continue;
      }

    trajMsg->GetTrajectoryElement(j, otherElemt);

    if (strcmp(elementName, otherElemt->GetName()) == 0)
      {
      // Same name
      std::stringstream newName;
      int suffix = 0;
      bool keepSearching = true;
      while(keepSearching)
	{
	newName << elementName << "_" << suffix;
	igtl::TrajectoryElement::Pointer temp;
	for (int k = 0; k < nOfTrajectories; ++k)
	  {
	  trajMsg->GetTrajectoryElement(k, temp);
	  if (newName.str().compare(temp->GetName()) == 0)
	    {
	    // Found another node with same newName
	    break;
	    }

	  if (k == nOfTrajectories-1)
	    {
	    // Didn't find node with new name
	    // Set new name and exit
	    otherElemt->SetName(newName.str().c_str());
	    keepSearching = false;
	    }
	  }
	suffix++;
	}
      }
    }
}
