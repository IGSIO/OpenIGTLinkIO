/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLLinearTransform.cxx $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLLinearTransform.h"

// OpenIGTLink includes
#include <igtlTransformMessage.h>

// MRML includes
#include <vtkMRMLLinearTransformNode.h>

#include <vtkMRMLModelNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLModelDisplayNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include <vtkAppendPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>


// VTKSYS includes
#include <vtksys/SystemTools.hxx>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLLinearTransform);
vtkCxxRevisionMacro(vtkIGTLToMRMLLinearTransform, "$Revision: 15552 $");


//---------------------------------------------------------------------------
vtkIGTLToMRMLLinearTransform::vtkIGTLToMRMLLinearTransform()
{
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLLinearTransform::~vtkIGTLToMRMLLinearTransform()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLLinearTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLLinearTransform::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLLinearTransformNode* transformNode;

  transformNode = vtkMRMLLinearTransformNode::New();
  transformNode->SetName(name);
  transformNode->SetDescription("Received by OpenIGTLink");

  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  transform->Identity();
  //transformNode->SetAndObserveImageData(transform);
  transformNode->ApplyTransformMatrix(transform);
  transform->Delete();

  vtkMRMLNode* n = scene->AddNode(transformNode);
  transformNode->Delete();

  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLLinearTransform::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);

  return events;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLLinearTransform::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkIGTLToMRMLBase::IGTLToMRML(buffer, node);

  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->Copy(buffer);  // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = transMsg->Unpack(this->CheckCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  if (node == NULL)
    {
    return 0;
    }

  vtkMRMLLinearTransformNode* transformNode =
    vtkMRMLLinearTransformNode::SafeDownCast(node);

  igtl::Matrix4x4 matrix;
  transMsg->GetMatrix(matrix);

  float tx = matrix[0][0];
  float ty = matrix[1][0];
  float tz = matrix[2][0];
  float sx = matrix[0][1];
  float sy = matrix[1][1];
  float sz = matrix[2][1];
  float nx = matrix[0][2];
  float ny = matrix[1][2];
  float nz = matrix[2][2];
  float px = matrix[0][3];
  float py = matrix[1][3];
  float pz = matrix[2][3];

  //std::cerr << "\n\nmatrix = " << std::endl;
  //std::cerr << tx << ", " << ty << ", " << tz << std::endl;
  //std::cerr << sx << ", " << sy << ", " << sz << std::endl;
  //std::cerr << nx << ", " << ny << ", " << nz << std::endl;
  //std::cerr << px << ", " << py << ", " << pz << std::endl;

  // set volume orientation
  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();

  transform->Identity();
  transform->Element[0][0] = tx;
  transform->Element[1][0] = ty;
  transform->Element[2][0] = tz;
  transform->Element[0][1] = sx;
  transform->Element[1][1] = sy;
  transform->Element[2][1] = sz;
  transform->Element[0][2] = nx;
  transform->Element[1][2] = ny;
  transform->Element[2][2] = nz;
  transform->Element[0][3] = px;
  transform->Element[1][3] = py;
  transform->Element[2][3] = pz;

  transformToParent->DeepCopy(transform);


  //std::cerr << "IGTL matrix = " << std::endl;
  //transform->Print(cerr);
  //std::cerr << "MRML matrix = " << std::endl;
  //transformToParent->Print(cerr);

  transform->Delete();

  return 1;

}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLLinearTransform::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (mrmlNode && event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {
    vtkMRMLLinearTransformNode* transformNode =
      vtkMRMLLinearTransformNode::SafeDownCast(mrmlNode);
    vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();

    //igtl::TransformMessage::Pointer OutTransformMsg;
    if (this->OutTransformMsg.IsNull())
      {
      this->OutTransformMsg = igtl::TransformMessage::New();
      }

    this->OutTransformMsg->SetDeviceName(mrmlNode->GetName());

    igtl::Matrix4x4 igtlmatrix;

    igtlmatrix[0][0]  = matrix->Element[0][0];
    igtlmatrix[1][0]  = matrix->Element[1][0];
    igtlmatrix[2][0]  = matrix->Element[2][0];
    igtlmatrix[3][0]  = matrix->Element[3][0];
    igtlmatrix[0][1]  = matrix->Element[0][1];
    igtlmatrix[1][1]  = matrix->Element[1][1];
    igtlmatrix[2][1]  = matrix->Element[2][1];
    igtlmatrix[3][1]  = matrix->Element[3][1];
    igtlmatrix[0][2]  = matrix->Element[0][2];
    igtlmatrix[1][2]  = matrix->Element[1][2];
    igtlmatrix[2][2]  = matrix->Element[2][2];
    igtlmatrix[3][2]  = matrix->Element[3][2];
    igtlmatrix[0][3]  = matrix->Element[0][3];
    igtlmatrix[1][3]  = matrix->Element[1][3];
    igtlmatrix[2][3]  = matrix->Element[2][3];
    igtlmatrix[3][3]  = matrix->Element[3][3];

    this->OutTransformMsg->SetMatrix(igtlmatrix);
    this->OutTransformMsg->Pack();

    *size = this->OutTransformMsg->GetPackSize();
    *igtlMsg = (void*)this->OutTransformMsg->GetPackPointer();

    return 1;
    }

  return 0;
}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLLinearTransform::SetVisibility(int sw, vtkMRMLScene * scene, vtkMRMLNode * node)
{
  vtkMRMLLinearTransformNode * tnode = vtkMRMLLinearTransformNode::SafeDownCast(node);

  if (!tnode || !scene)
    {
    // If the node is not a linear transform node, do nothing.
    return;
    }

  vtkMRMLModelNode*   locatorModel = NULL;
  vtkMRMLDisplayNode* locatorDisp  = NULL;
  
  const char * attr = tnode->GetAttribute("IGTLModelID");
  if (!attr || !scene->GetNodeByID(attr)) // no locator has been created
    {
    if (sw)
      {
      std::stringstream ss;
      ss << "Locator_" << tnode->GetName();
      locatorModel = AddLocatorModel(scene, ss.str().c_str(), 0.0, 1.0, 1.0);
      if (locatorModel)
        {
        tnode->SetAttribute("IGTLModelID", locatorModel->GetID());
        scene->Modified();
        locatorModel->SetAndObserveTransformNodeID(tnode->GetID());
        locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
        }
      }
    else
      {
      locatorModel = NULL;
      }
    }
  else
    {
    locatorModel = vtkMRMLModelNode::SafeDownCast(scene->GetNodeByID(attr));
    }
  if (locatorModel)
    {
    locatorDisp = locatorModel->GetDisplayNode();
    locatorDisp->SetVisibility(sw);
    locatorModel->Modified();
    }
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkIGTLToMRMLLinearTransform::AddLocatorModel(vtkMRMLScene * scene, const char* nodeName, double r, double g, double b)
{

  vtkMRMLModelNode           *locatorModel;
  vtkMRMLModelDisplayNode    *locatorDisp;

  locatorModel = vtkMRMLModelNode::New();
  locatorDisp = vtkMRMLModelDisplayNode::New();

  // Cylinder represents the locator stick
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius(1.5);
  cylinder->SetHeight(100);
  cylinder->SetCenter(0, 0, 0);
  cylinder->Update();

  // Rotate cylinder
  vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans =   vtkTransform::New();
  trans->RotateX(90.0);
  trans->Translate(0.0, -50.0, 0.0);
  trans->Update();
  tfilter->SetInput(cylinder->GetOutput());
  tfilter->SetTransform(trans);
  tfilter->Update();

  // Sphere represents the locator tip
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();

  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  //apd->AddInput(cylinder->GetOutput());
  apd->AddInput(tfilter->GetOutput());
  apd->Update();

  locatorModel->SetAndObservePolyData(apd->GetOutput());

  double color[3];
  color[0] = r;
  color[1] = g;
  color[2] = b;
  locatorDisp->SetPolyData(locatorModel->GetPolyData());
  locatorDisp->SetColor(color);

  trans->Delete();
  tfilter->Delete();
  cylinder->Delete();
  sphere->Delete();
  apd->Delete();

  scene->SaveStateForUndo();
  scene->AddNode(locatorDisp);
  vtkMRMLNode* lm = scene->AddNode(locatorModel);
  locatorDisp->SetScene(scene);
  locatorModel->SetName(nodeName);
  locatorModel->SetScene(scene);
  locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
  locatorModel->SetHideFromEditors(0);

  locatorModel->Delete();
  locatorDisp->Delete();

  return vtkMRMLModelNode::SafeDownCast(lm);

}
