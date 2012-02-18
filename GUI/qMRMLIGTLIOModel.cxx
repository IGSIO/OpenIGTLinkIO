/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

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
#include <QTimer>

// qMRML includes
#include "qMRMLIGTLIOModel.h"

// MRML includes
#include <vtkMRMLDisplayableHierarchyNode.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCollection.h>

// OpenIGTLink IF includes
#include "vtkMRMLIGTLConnectorNode.h"

#include "qMRMLIGTLIOModel_p.h"

// TODO: reimplement functions that use mrmlNodeFromItem() in qMRMLSceneModel.
//  1. updateNodeFromItem()     -- virtual 
//  2. onMRMLSceneNodeRemoved()   -- virtual
//  3. anything else?

//------------------------------------------------------------------------------
qMRMLIGTLIOModelPrivate::qMRMLIGTLIOModelPrivate(qMRMLIGTLIOModel& object)
  : qMRMLSceneModelPrivate(object)
{
}


//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModelPrivate::insertExtraItem2(int row, QStandardItem* parent,
                                              const QString& text,
                                              const QString& extraType,
                                              const Qt::ItemFlags& flags)
{
  Q_ASSERT(parent);

  QStandardItem* item = new QStandardItem;
  item->setData(extraType, qMRMLSceneModel::UIDRole);
  if (text == "separator")
    {
    item->setData("separator", Qt::AccessibleDescriptionRole);
    }
  else
    {
    item->setText(text);
    }
  item->setFlags(flags);
  QList<QStandardItem*> items;
  items << item;

  for (int i = 0; i < 3; i ++)
    {
    QStandardItem* item0 = new QStandardItem;
    item0->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    items << item0;
    }
  parent->insertRow(row, items);

  // update extra item cache info (for faster retrieval)
  QMap<QString, QVariant> extraItems = parent->data(qMRMLSceneModel::ExtraItemsRole).toMap();
  extraItems[extraType] = extraItems[extraType].toStringList() << text;
  parent->setData(extraItems, qMRMLSceneModel::ExtraItemsRole );

  return item;
}


//------------------------------------------------------------------------------
qMRMLIGTLIOModel::qMRMLIGTLIOModel(QObject *vparent)
  :Superclass(new qMRMLIGTLIOModelPrivate(*this), vparent)
{
  Q_D(qMRMLIGTLIOModel);
  d->init(/*new qMRMLSceneModelItemHelperFactory*/);

  this->setListenNodeModifiedEvent(true);
  this->setIDColumn(-1);
  this->setCheckableColumn(qMRMLIGTLIOModel::VisualizationColumn);
  this->setColumnCount(4);
  this->setHorizontalHeaderLabels(QStringList() << "Name" << "MRML Type" << "IGTL Type" << "Vis");

  // Hack: disconnect signal and slot to prevent this->mrmlNodeFromItem(item) call
  // that returns NULL. (in onItemChanged())
  QObject::disconnect(this, SIGNAL(itemChanged(QStandardItem*)),
                      this, SLOT(onItemChanged(QStandardItem*)));

}

//------------------------------------------------------------------------------
qMRMLIGTLIOModel::qMRMLIGTLIOModel(qMRMLIGTLIOModelPrivate* pimpl, QObject *parent)
  :Superclass(pimpl, parent)
{
}


//------------------------------------------------------------------------------
qMRMLIGTLIOModel::~qMRMLIGTLIOModel()
{

}


//-----------------------------------------------------------------------------
/// Set and observe the logic
//-----------------------------------------------------------------------------
void qMRMLIGTLIOModel::setLogic(vtkSlicerOpenIGTLinkIFLogic* logic)
{
  if (!logic)
    {
    return;
    }
  this->Logic = logic;
}


//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModel::insertNode(vtkMRMLNode* node, QStandardItem* parent, int row)
{
  QStandardItem* insertedItem = this->Superclass::insertNode(node, parent, row);
  if (this->listenNodeModifiedEvent() &&
      node->IsA("vtkMRMLIGTLConnectorNode"))
    {
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::ConnectedEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::DisconnectedEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::ActivatedEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::DeactivatedEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::NewDeviceEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    qvtkConnect(node, vtkMRMLIGTLConnectorNode::DeviceModifiedEvent,
                this, SLOT(onDeviceVisibilityModified(vtkObject*)));

    }
  return insertedItem;
}


//------------------------------------------------------------------------------
//void qMRMLIGTLIOModel::updateItemFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
void qMRMLIGTLIOModel::updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
{
  //Q_D(qMRMLIGTLIOModel);

  //this->Superclass::updateItemFromNode(item, node, column);
  vtkMRMLIGTLConnectorNode* cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return;
    }

  switch (column)
    {
    case qMRMLIGTLIOModel::NameColumn:
      {
      item->setText(cnode->GetName());
      insertIOTree(cnode);
      break;
      }
    case qMRMLIGTLIOModel::TypeColumn:
      {
      Q_ASSERT(cnode->GetType() < vtkMRMLIGTLConnectorNode::NUM_TYPE);
      //item->setText(QString(vtkMRMLIGTLConnectorNode::ConnectorTypeStr[cnode->GetType()]));
      item->setText("");
      break;
      }
    case qMRMLIGTLIOModel::StatusColumn:
      {
      Q_ASSERT(cnode->GetState() < vtkMRMLIGTLConnectorNode::NUM_STATE);
      //item->setText(QString(vtkMRMLIGTLConnectorNode::ConnectorStateStr[cnode->GetState()]));
      item->setText("");
      break;
      }
    default:
      break;
    }
}


//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateNodeFromItem(vtkMRMLNode* node, QStandardItem* item)
{
  // updateNodeFromItem is reimplemented, since the base class cannot handle
  // a function call with node=NULL

  if (node == NULL)
    {
    return;
    }

  int wasModifying = node->StartModify();
  this->updateNodeFromItemData(node, item);
  node->EndModify(wasModifying);

  // the following only applies to tree hierarchies
  if (!this->canBeAChild(node))
    {
    return;
    }

 Q_ASSERT(node != this->mrmlNodeFromItem(item->parent()));

  QStandardItem* parentItem = item->parent();

  // Don't do the following if the row is not complete (reparenting an
  // incomplete row might lead to errors). (if there is no child yet for a given
  // column, it will get there next time updateNodeFromItem is called).
  // updateNodeFromItem() is called for every item drag&dropped (we insure that
  // all the indexes of the row are reparented when entering the d&d function
  for (int i = 0; i < parentItem->columnCount(); ++i)
    {
    if (parentItem->child(item->row(), i) == 0)
      {
      return;
      }
    }

  vtkMRMLNode* parent = this->mrmlNodeFromItem(parentItem);
  int desiredNodeIndex = -1;
  if (this->parentNode(node) != parent)
    {
    this->reparent(node, parent);
    }
  else if ((desiredNodeIndex = this->nodeIndex(node)) != item->row())
    {
    QStandardItem* parentItem = item->parent();
    if (parentItem && desiredNodeIndex <
          (parentItem->rowCount() - this->postItems(parentItem).count()))
      {
      this->updateItemFromNode(item, node, item->column());
      }
    }
}



//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::addNodeToCurrentBranch(vtkMRMLNode* node)
{
  //Q_D(qMRMLIGTLIOModel);
}


//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateIOTreeBranch(vtkMRMLIGTLConnectorNode* node, QStandardItem* item, qMRMLIGTLIOModel::Direction dir)
{
  // List incoming data
  if (node == NULL)
    {
    return;
    }

  int nnodes;      
  if (dir == qMRMLIGTLIOModel::INCOMING)
    {
    nnodes = node->GetNumberOfIncomingMRMLNodes();
    }
  else
    {
    nnodes = node->GetNumberOfOutgoingMRMLNodes();
    }

  for (int i = 0; i < nnodes; i ++)
    {
    vtkMRMLNode* inode;
    if (dir == qMRMLIGTLIOModel::INCOMING)
      {
      inode = node->GetIncomingMRMLNode(i);
      }
    else
      {
      inode = node->GetOutgoingMRMLNode(i);
      }

    if (inode != NULL)
      {
      int row = -1;

      // Check if the node is already added
      int nRows = item->rowCount();
      for (int r = 0; r < nRows; r ++)
        {
        QStandardItem* c = item->child(r, 0);
        if (c)
          {
          QString text = c->data().toString();
          if (text.compare(QString(inode->GetID())) == 0)
            {
            // Found the node... skip
            row = r;
            break;
            }
          }
        }
      
      if (row < 0) // If the node is not in the tree, add it.
        {
        QList<QStandardItem*> items;
        // Node name
        QStandardItem* item0 = new QStandardItem;
        item0->setText(inode->GetName());
        item0->setData(QString(inode->GetID()), qMRMLSceneModel::UIDRole);
        item0->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        items << item0;
        
        // Node tag name
        QStandardItem* item1 = new QStandardItem;
        item1->setText(inode->GetNodeTagName());
        item1->setData(QString(inode->GetID()), qMRMLSceneModel::UIDRole);
        item1->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        items << item1;
        
        // IGTL name
        QStandardItem* item2 = new QStandardItem;
        item2->setData(QString(inode->GetID()), qMRMLSceneModel::UIDRole);
        item2->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        vtkIGTLToMRMLBase* converter = node->GetConverterByNodeID(inode->GetID());
        if (converter)
          {
          item2->setText(converter->GetIGTLName());
          }
        else
          {
          item2->setText("--");
          }
        items << item2;

        // Visibility icon
        QStandardItem* item3 = new QStandardItem;
        const char * attr = inode->GetAttribute("IGTLVisible");
        if (attr && strcmp(attr, "true") == 0)
          {
          item3->setData(QPixmap(":/Icons/Small/SlicerVisible.png"),Qt::DecorationRole);        
          }
        else
          {
          item3->setData(QPixmap(":/Icons/Small/SlicerInvisible.png"),Qt::DecorationRole);
          }
        item3->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        items << item3;
        
        item->insertRow(i, items);
        }
      else // If the node is in the tree, only update visibility icon
        {
        QStandardItem* item3 = item->child(row, 3);
        if (item3)
          {
          const char * attr = inode->GetAttribute("IGTLVisible");
          if (attr && strcmp(attr, "true") == 0)
            {
            item3->setData(QPixmap(":/Icons/Small/SlicerVisible.png"),Qt::DecorationRole);        
            }
          else
            {
            item3->setData(QPixmap(":/Icons/Small/SlicerInvisible.png"),Qt::DecorationRole);
            }
          }
        }
      }

    //// update extra item cache info (for faster retrieval)
    //QMap<QString, QVariant> extraItems = nodeItem->data(qMRMLSceneModel::ExtraItemsRole).toMap();
    //extraItems[extraType] = extraItems[extraType].toStringList() << text;
    //parent->setData(extraItems, qMRMLSceneModel::ExtraItemsRole );
    }
}


//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModel::insertIOTree(vtkMRMLNode* node)
{
  Q_D(qMRMLIGTLIOModel);

  // Check if the node is a connector node
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (!cnode)
    {
    return NULL;
    }
  
  QStandardItem* nodeItem = this->itemFromNode(node);
  if (nodeItem == 0 || d == 0)
    {
    return NULL;
    }

  //d->removeAllExtraItems(nodeItem, "IOTree");
  QStandardItem* inItem;
  QStandardItem* outItem;
  if (nodeItem->rowCount() == 0)
    {
    inItem = d->insertExtraItem2(nodeItem->rowCount(), nodeItem,
                                 QString("IN"), "IOTree", Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    outItem = d->insertExtraItem2(nodeItem->rowCount(), nodeItem,
                                  QString("OUT"), "IOTree", Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    }
  else
    {
    inItem = nodeItem->child(0,0);
    outItem = nodeItem->child(1,0);
    }
  updateIOTreeBranch(cnode, inItem, qMRMLIGTLIOModel::INCOMING);
  updateIOTreeBranch(cnode, outItem, qMRMLIGTLIOModel::OUTGOING);  
  
  return nodeItem;

}


//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModel::insertNode(vtkMRMLNode* node)
{
  //return this->Superclass::insertNode(node);

  Q_D(qMRMLIGTLIOModel);
  QStandardItem* nodeItem = this->itemFromNode(node);
  if (nodeItem != 0)
    {
    // It is possible that the node has been already added if it is the parent
    // of a child node already inserted.
    return nodeItem;
    }
  vtkMRMLNode* parentNode = this->parentNode(node);
  QStandardItem* parentItem =
    parentNode ? this->itemFromNode(parentNode) : this->mrmlSceneItem();
  if (!parentItem)
    {
    Q_ASSERT(parentNode);
    parentItem = this->insertNode(parentNode);
    Q_ASSERT(parentItem);
    }
  int min = this->preItems(parentItem).count();
  int max = parentItem->rowCount() - this->postItems(parentItem).count();
  int row = min + this->nodeIndex(node);
  if (row > max)
    {
    d->MisplacedNodes << node;
    row = max;
    }
  nodeItem = this->insertNode(node, parentItem, row);
  Q_ASSERT(this->itemFromNode(node) == nodeItem);

  insertIOTree(node);

  return nodeItem;

}


//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onDeviceVisibilityModified(vtkObject* obj)
{
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(obj);

  if (cnode)
    {
    insertIOTree(cnode);
    }
}


//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeAdded(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  // process only if node is vtkMRMLIGTLConnectorNode.
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    qMRMLSceneModel::onMRMLSceneNodeAdded(scene, node);
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeAboutToBeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    qMRMLSceneModel::onMRMLSceneNodeAboutToBeRemoved(scene, node);
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    qMRMLSceneModel::onMRMLSceneNodeRemoved(scene, node);
    }
}
