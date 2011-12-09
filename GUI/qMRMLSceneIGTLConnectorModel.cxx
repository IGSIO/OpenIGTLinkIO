/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QMap>
#include <QMimeData>
#include <QSharedPointer>
#include <QStack>
#include <QStringList>
#include <QVector>

// OpenIGTLinkIF GUI includes
#include "qMRMLSceneIGTLConnectorModel.h"

// qMRMLWidgets includes
#include <qMRMLSceneDisplayableModel.h>

// OpenIGTLinkIF MRML includes
#include "vtkMRMLIGTLConnectorNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLAnnotationNode.h>
#include <vtkMRMLDisplayableHierarchyNode.h>

// VTK includes
#include <vtkVariantArray.h>

// STD includes
#include <typeinfo>

//------------------------------------------------------------------------------
qMRMLSceneIGTLConnectorModel::qMRMLSceneIGTLConnectorModel(QObject *vparent)
  :qMRMLSceneDisplayableModel(vparent)
{
  this->setListenNodeModifiedEvent(true);
  this->setNameColumn(-1);
  this->setIDColumn(-1);
  //this->setCheckableColumn(qMRMLSceneIGTLConnectorModel::CheckedColumn);

  this->setColumnCount(6);
  this->setHorizontalHeaderLabels(
    QStringList() << "" << "Name" << "Type" << "Status" << "Destination");
}

//------------------------------------------------------------------------------
qMRMLSceneIGTLConnectorModel::~qMRMLSceneIGTLConnectorModel()
{
}

//------------------------------------------------------------------------------
void qMRMLSceneIGTLConnectorModel::updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item)
{
  int oldChecked = node->GetSelected();
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return;
    }

  this->qMRMLSceneDisplayableModel::updateNodeFromItemData(node, item);

  switch (item->column())
    {
    case qMRMLSceneIGTLConnectorModel::NameColumn:
      {
      cnode->SetName(item->text().toLatin1());
      break;
      }
    case qMRMLSceneIGTLConnectorModel::TypeColumn:
      {
      }
    case qMRMLSceneIGTLConnectorModel::StatusColumn:
      {
      }
    case qMRMLSceneIGTLConnectorModel::AddressColumn:
      {
      }
    case qMRMLSceneIGTLConnectorModel::PortColumn:
      {
      }
    default:
      break;
    }

}

//------------------------------------------------------------------------------
void qMRMLSceneIGTLConnectorModel::updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
{
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);

  if (!cnode)
    {
    return;
    }

  switch (column)
    {
    case qMRMLSceneIGTLConnectorModel::NameColumn:
      {
      item->setText(QString(cnode->GetName()));
      break;
      }
    case qMRMLSceneIGTLConnectorModel::TypeColumn:
      {
      if (cnode->GetType() == vtkMRMLIGTLConnectorNode::TYPE_SERVER)
        {
        item->setText(QString("S"));
        }
      else if (cnode->GetType() == vtkMRMLIGTLConnectorNode::TYPE_CLIENT)
        {
        item->setText(QString("C"));
        }
      else
        {
        item->setText(QString("?"));
        }
      break;
      }
    case qMRMLSceneIGTLConnectorModel::StatusColumn:
      {
      if (cnode->GetState() == vtkMRMLIGTLConnectorNode::STATE_OFF)
        {
        item->setText(QString("OFF"));
        }
      else if (cnode->GetState() == vtkMRMLIGTLConnectorNode::STATE_WAIT_CONNECTION)
        {
        item->setText(QString("WAIT"));
        }
      else if (cnode->GetState() == vtkMRMLIGTLConnectorNode::STATE_CONNECTED)
        {
        item->setText(QString("ON"));
        }
      else
        {
        item->setText(QString("--"));
        }
      break;
      }
    case qMRMLSceneIGTLConnectorModel::AddressColumn:
      {
      if (cnode->GetType() == vtkMRMLIGTLConnectorNode::TYPE_SERVER)
        {
        item->setText(QString(cnode->GetServerHostname()));
        }
      else
        {
        item->setText(QString("--"));
        }
      break;
      }
    case qMRMLSceneIGTLConnectorModel::PortColumn:
      {
      std::stringstream ss;
      ss << cnode->GetServerPort();
      item->setText(QString(ss.str().c_str()));
      break;
      }
    default:
      break;
    }

}

//------------------------------------------------------------------------------
QFlags<Qt::ItemFlag> qMRMLSceneIGTLConnectorModel::nodeFlags(vtkMRMLNode* node, int column)const
{
  QFlags<Qt::ItemFlag> flags = this->qMRMLSceneDisplayableModel::nodeFlags(node, column);
  // remove the ItemIsEditable flag from any possible item (typically at column 0)
  flags = flags & ~Qt::ItemIsEditable;
  // and set it to the right column
  switch(column)
    {
    //case qMRMLSceneIGTLConnectorModel::TextColumn:
    //  flags = flags | Qt::ItemIsEditable;
    //  break;
    default:
      break;
    }
  return flags;
}

//------------------------------------------------------------------------------
vtkMRMLNode* qMRMLSceneIGTLConnectorModel::parentNode(vtkMRMLNode* node)const
{
  if (node == NULL)
    {
    return 0;
    }

  // MRML Displayable nodes (inherits from transformable)
  vtkMRMLDisplayableNode *displayableNode = vtkMRMLDisplayableNode::SafeDownCast(node);
  vtkMRMLDisplayableHierarchyNode * displayableHierarchyNode = NULL;
  if (displayableNode &&
      displayableNode->GetScene() &&
      displayableNode->GetID())
    {
    // get the displayable hierarchy node associated with this displayable node
    displayableHierarchyNode = vtkMRMLDisplayableHierarchyNode::GetDisplayableHierarchyNode(displayableNode->GetScene(), displayableNode->GetID());

    if (displayableHierarchyNode)
      {
      if (displayableHierarchyNode->GetHideFromEditors())
        {
        // this is a hidden hierarchy node, so we do not want to display it
        // instead, we will return the parent of the hidden hierarchy node
        // to be used as the parent for the displayableNode
        return displayableHierarchyNode->GetParentNode();
        }
      return displayableHierarchyNode;
      }
    }
  if (displayableHierarchyNode == NULL)
    {
    // the passed in node might have been a hierarchy node instead, try to
    // cast it
    displayableHierarchyNode = vtkMRMLDisplayableHierarchyNode::SafeDownCast(node);
    }
  if (displayableHierarchyNode)
    {
    // return it's parent
    return displayableHierarchyNode->GetParentNode();
    }
  return 0;
}


//-----------------------------------------------------------------------------
/// Set and observe the logic
//-----------------------------------------------------------------------------
void qMRMLSceneIGTLConnectorModel::setLogic(vtkSlicerOpenIGTLinkIFLogic* logic)
{
  if (!logic)
    {
    return;
    }

  this->m_Logic = logic;

}
