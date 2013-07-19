/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLPointMetaList.cxx $
  Date:      $Date: 2009-10-05 17:37:20 -0400 (Mon, 05 Oct 2009) $
  Version:   $Revision: 10577 $

==========================================================================*/

#include "vtkIGTLToMRMLPointMetaList.h"

#include "vtkImageData.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "igtlPointMessage.h"

#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLPointMetaListNode.h"
#include "vtkMRMLIGTLQueryNode.h"

// VTK includes
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkIGTLToMRMLPointMetaList);
vtkCxxRevisionMacro(vtkIGTLToMRMLPointMetaList, "$Revision: 10577 $");


//---------------------------------------------------------------------------
vtkIGTLToMRMLPointMetaList::vtkIGTLToMRMLPointMetaList()
{
std::cout << "initialized vtkIGTLToMRMLPointMetaList" << std::endl;
}


//---------------------------------------------------------------------------
vtkIGTLToMRMLPointMetaList::~vtkIGTLToMRMLPointMetaList()
{
}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLPointMetaList::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLPointMetaList::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLPointMetaListNode *node = vtkMRMLPointMetaListNode::New();
  node->SetName(name);
  node->SetDescription("Received by OpenIGTLink");

  scene->AddNode(node);
  node->Delete();
  return node;
}


//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLPointMetaList::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLPointNode::ModifiedEvent); 

  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPointMetaList::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  std::cout << " vtkIGTLToMRMLPointMetaList->IGTLToMRML " << std::endl;
  if (strcmp(node->GetNodeTagName(), this->GetMRMLName()) != 0)
    {
    std::cerr << "Invalide node. NodeTagName: " << node->GetNodeTagName() << " MRMLName: " << this->GetMRMLName() << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Allocate Point Message Class

  // Create a message buffer to receive transform data
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->Copy(buffer);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = pointMsg->Unpack();
	std::cout << "crc check result: " << c << std::endl;
  //if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  if (igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
		std::cout << "inside CRC check" << std::endl;
    vtkMRMLPointMetaListNode* pmnode = vtkMRMLPointMetaListNode::SafeDownCast(node);
		std::cout << "    attempting crash: " << std::endl; 
		std::cout << "     " << pmnode->GetID() << std::endl;
		std::cout << "got past SafeDownCast" << std::endl;
    if (pmnode)
      {
			std::cout << "inside node check" << std::endl;
      int modid = pmnode->StartModify();
      int nElements = pointMsg->GetNumberOfPointElement();
      for (int i = 0; i < nElements; i ++)
        {
        igtl::PointElement::Pointer pointElement;
        pointMsg->GetPointElement(i, pointElement);
       
        igtlUint8 rgba[4];
        pointElement->GetRGBA(rgba);
        
        igtlFloat32 pos[3];
        pointElement->GetPosition(pos);
       
				vtkMRMLPointMetaListNode::PointMetaElement element;
				element.Name 				= pointElement->GetName();
				element.GroupName 	= pointElement->GetGroupName();
				element.Radius			= pointElement->GetRadius();
				
				element.RGBA[0]			= rgba[0];
				element.RGBA[1]			= rgba[1];
				element.RGBA[2]			= rgba[2];
				element.RGBA[3]			= rgba[3];

				element.Position[0] = pos[0];
				element.Position[1] = pos[1];
				element.Position[2] = pos[2];
				element.Owner				= pointElement->GetOwner();
				
				pmnode->AddPointMetaElement(element);
					
        std::cerr << "========== Element #" << i << " ==========" << std::endl;
        std::cerr << " Name      : " << pointElement->GetName() << std::endl;
        std::cerr << " GroupName : " << pointElement->GetGroupName() << std::endl;
        std::cerr << " RGBA      : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
        std::cerr << " Position  : ( " << std::fixed << pos[0] << ", " << pos[1] << ", " << pos[2] << " )" << std::endl;
        std::cerr << " Radius    : " << std::fixed << pointElement->GetRadius() << std::endl;
        std::cerr << " Owner     : " << pointElement->GetOwner() << std::endl;
        std::cerr << "================================" << std::endl;

        }
      pmnode->EndModify(modid);
			std::cerr << "PointMetaList contains: " << pmnode->GetNumberOfPointMetaElement() << "  points" << pmnode->GetID() << std::endl;
    	pmnode->Modified();
      }
    }
  return 1;

}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPointMetaList::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  if ((event == vtkMRMLFiducialListNode::FiducialModifiedEvent ||
       event == vtkMRMLFiducialListNode::FiducialIndexModifiedEvent ||
       event == vtkMRMLFiducialListNode::DisplayModifiedEvent)
      && strcmp(mrmlNode->GetNodeTagName(), this->GetMRMLName()) == 0)
    {
    
    }
  // If mrmlNode is query node
  else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0 ) // Query Node
    {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
      {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
       {
       if (this->OutPointMsg.IsNull())
          {
          this->OutPointMsg = igtl::PointMessage::New();
          }
        if (qnode->GetNoNameQuery() == 1)
          {
          this->OutPointMsg->SetDeviceName("");
          }
        else
          {
          this->OutPointMsg->SetDeviceName(mrmlNode->GetName());
          }
				std::cout << "built OutPointMsg" << std::endl;
				std::cout << " DeviceName: " << this->OutPointMsg->GetDeviceName() << std::endl;
				std::cout << " DeviceType: " << this->OutPointMsg->GetDeviceType() << std::endl;
				std::cout << " BodyType: " << this->OutPointMsg->GetBodyType() << std::endl;
				this->OutPointMsg->Pack();
        *size = this->OutPointMsg->GetPackSize();

	      *igtlMsg = this->OutPointMsg->GetPackPointer();
        return 1;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
        {
        // N / A
        return 0;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
        {
        // N/  A
        return 0;
        }
      return 0;
      }
    else
      {
      return 0;
      }
    }
  return 0;
}



