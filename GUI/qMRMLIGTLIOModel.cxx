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
#include "qMRMLIGTLIOModel_p.h"

// MRML includes
#include <vtkMRMLDisplayableHierarchyNode.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkCollection.h>

// STD includes

//------------------------------------------------------------------------------
// qMRMLIGTLIOModel
//------------------------------------------------------------------------------
qMRMLIGTLIOModel::qMRMLIGTLIOModel(QObject *_parent)
  :qMRMLSceneModel(_parent)
  , d_ptr(new qMRMLIGTLIOModelPrivate(*this))
{
  Q_D(qMRMLIGTLIOModel);
  d->init(/*new qMRMLIGTLIOModelItemHelperFactory*/);
}

//------------------------------------------------------------------------------
qMRMLIGTLIOModel::qMRMLIGTLIOModel(qMRMLIGTLIOModelPrivate* pimpl, QObject *parentObject)
  :qMRMLSceneModel(parentObject)
  , d_ptr(pimpl)
{
  Q_D(qMRMLIGTLIOModel);
  d->init(/*factory*/);
}

//------------------------------------------------------------------------------
qMRMLIGTLIOModel::~qMRMLIGTLIOModel()
{
}




//------------------------------------------------------------------------------
vtkMRMLNode* qMRMLIGTLIOModel::parentNode(vtkMRMLNode* node)const
{
  Q_UNUSED(node);
  return 0;
}

//------------------------------------------------------------------------------
int qMRMLIGTLIOModel::nodeIndex(vtkMRMLNode* node)const
{
  Q_D(const qMRMLIGTLIOModel);
  if (!d->MRMLScene)
    {
    return -1;
    }
  const char* nodeId = node ? node->GetID() : 0;
  if (nodeId == 0)
    {
    return -1;
    }
  const char* nId = 0;
  int index = -1;
  vtkMRMLNode* parent = this->parentNode(node);

  // otherwise, iterate through the scene
  vtkCollection* nodes = d->MRMLScene->GetNodes();
  vtkMRMLNode* n = 0;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it);
       (n = (vtkMRMLNode*)nodes->GetNextItemAsObject(it)) ;)
    {
    // note: parent can be NULL, it means that the scene is the parent
    if (parent == this->parentNode(n))
      {
      ++index;
      nId = n->GetID();
      if (nId && !strcmp(nodeId, nId))
        {
        return index;
        }
      }
    }
  return -1;
}

//------------------------------------------------------------------------------
bool qMRMLIGTLIOModel::canBeAChild(vtkMRMLNode* node)const
{
  Q_UNUSED(node);
  return false;
}

//------------------------------------------------------------------------------
bool qMRMLIGTLIOModel::canBeAParent(vtkMRMLNode* node)const
{
  Q_UNUSED(node);
  return false;
}

//------------------------------------------------------------------------------
bool qMRMLIGTLIOModel::reparent(vtkMRMLNode* node, vtkMRMLNode* newParent)
{
  Q_UNUSED(node);
  Q_UNUSED(newParent);
  return false;
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateScene()
{
  Q_D(qMRMLIGTLIOModel);

  // Stop listening to all the nodes before we remove them (setRowCount) as some
  // weird behavior could arise when removing the nodes (e.g onMRMLNodeModified
  // could be called ...)
  qvtkDisconnect(0, vtkCommand::ModifiedEvent,
                 this, SLOT(onMRMLNodeModified(vtkObject*)));

  // Enabled so it can be interacted with
  this->invisibleRootItem()->setFlags(Qt::ItemIsEnabled);

  // Extra items before the scene item. Typically there is no top-level extra
  // items, only at the Scene level (before and after the nodes)
  const int preSceneItemCount = this->preItems(0).count();
  const int postSceneItemCount = this->postItems(0).count();

  if (!this->mrmlSceneItem() && d->MRMLScene)
    {
    // No scene item has been created yet, but the MRMLScene is valid so we
    // need to create one.
    QList<QStandardItem*> sceneItems;
    QStandardItem* sceneItem = new QStandardItem;
    sceneItem->setFlags(Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    sceneItem->setText("Scene");
    sceneItem->setData("scene", qMRMLIGTLIOModel::UIDRole);
    sceneItems << sceneItem;
    for (int i = 1; i < this->columnCount(); ++i)
      {
      QStandardItem* sceneOtherColumn = new QStandardItem;
      sceneOtherColumn->setFlags(0);
      sceneItems << sceneOtherColumn;
      }
    this->insertRow(preSceneItemCount, sceneItems);
    }
  else if (!d->MRMLScene)
    {
    // TBD: Because we don't call clear, I don't think restoring the column
    // count is necessary because it shouldn't be changed.
    const int oldColumnCount = this->columnCount();
    this->removeRows(
      preSceneItemCount,
      this->rowCount() - preSceneItemCount - postSceneItemCount);
    this->setColumnCount(oldColumnCount);
    return;
    }

  // Update the scene pointer in case d->MRMLScene has changed
  this->mrmlSceneItem()->setData(
    QVariant::fromValue(reinterpret_cast<long long>(d->MRMLScene)),
    qMRMLIGTLIOModel::PointerRole);

  const int preNodesItemCount = this->preItems(this->mrmlSceneItem()).count();
  const int postNodesItemCount = this->postItems(this->mrmlSceneItem()).count();
  // Just remove the nodes, not the extra items like "None", "Create node" etc.
  this->mrmlSceneItem()->removeRows(
    preNodesItemCount,
    this->mrmlSceneItem()->rowCount() - preNodesItemCount - postNodesItemCount);

  // Populate scene with nodes
  this->populateScene();
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::populateScene()
{
  Q_D(qMRMLIGTLIOModel);
  Q_ASSERT(d->MRMLScene);
  // Add nodes
  vtkMRMLNode *node = 0;
  vtkCollectionSimpleIterator it;
  d->MisplacedNodes.clear();
  for (d->MRMLScene->GetNodes()->InitTraversal(it);
       (node = (vtkMRMLNode*)d->MRMLScene->GetNodes()->GetNextItemAsObject(it)) ;)
    {
    this->insertNode(node);
    }
  foreach(vtkMRMLNode* misplacedNode, d->MisplacedNodes)
    {
    this->onMRMLNodeModified(misplacedNode);
    }
}

//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModel::insertNode(vtkMRMLNode* node)
{
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
  return nodeItem;
}

//------------------------------------------------------------------------------
QStandardItem* qMRMLIGTLIOModel::insertNode(vtkMRMLNode* node, QStandardItem* parent, int row)
{
  Q_D(qMRMLIGTLIOModel);
  Q_ASSERT(vtkMRMLNode::SafeDownCast(node));

  QList<QStandardItem*> items;
  for (int i= 0; i < this->columnCount(); ++i)
    {
    QStandardItem* newNodeItem = new QStandardItem();
    this->updateItemFromNode(newNodeItem, node, i);
    items.append(newNodeItem);
    }
  if (parent)
    {
    parent->insertRow(row, items);
    //Q_ASSERT(parent->columnCount() == 2);
    }
  else
    {
    this->insertRow(row,items);
    }
  // TODO: don't listen to nodes that are hidden from editors ?
  if (d->ListenNodeModifiedEvent)
    {
    qvtkConnect(node, vtkCommand::ModifiedEvent,
                this, SLOT(onMRMLNodeModified(vtkObject*)));
    }
  return items[0];
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateItemFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
{
  Q_D(qMRMLIGTLIOModel);
  // We are going to make potentially multiple changes to the item. We want to
  // refresh the node only once, so we "block" the updates in onItemChanged().
  d->PendingItemModified = 0;
  item->setFlags(this->nodeFlags(node, column));
  // set UIDRole and set PointerRole need to be atomic
  bool blocked  = this->blockSignals(true);
  item->setData(QString(node->GetID()), qMRMLIGTLIOModel::UIDRole);
  item->setData(QVariant::fromValue(reinterpret_cast<long long>(node)), qMRMLIGTLIOModel::PointerRole);
  this->blockSignals(blocked);
  this->updateItemDataFromNode(item, node, column);

  bool itemChanged = (d->PendingItemModified > 0);
  d->PendingItemModified = -1;

  if (this->canBeAChild(node))
    {
    QStandardItem* parentItem = item->parent();
    QStandardItem* newParentItem = this->itemFromNode(this->parentNode(node));
    if (newParentItem == 0)
      {
      newParentItem = this->mrmlSceneItem();
      }
    // If the item has no parent, then it means it hasn't been put into the scene yet.
    // and it will do it automatically.
    if (parentItem)
      {
      int newIndex = this->nodeIndex(node);
      if (parentItem != newParentItem ||
          newIndex != item->row())
        {
        QList<QStandardItem*> children = parentItem->takeRow(item->row());
        d->reparentItems(children, newIndex, newParentItem);
        }
      }
    }
  if (itemChanged)
    {
    this->onItemChanged(item);
    }
}

//------------------------------------------------------------------------------
QFlags<Qt::ItemFlag> qMRMLIGTLIOModel::nodeFlags(vtkMRMLNode* node, int column)const
{
  QFlags<Qt::ItemFlag> flags = Qt::ItemIsEnabled
                             | Qt::ItemIsSelectable;
  if (column == this->checkableColumn() && node->GetSelectable())
    {
    flags = flags | Qt::ItemIsUserCheckable;
    }
  if (column == this->nameColumn())
    {
    flags = flags | Qt::ItemIsEditable;
    }
  if (this->canBeAChild(node))
    {
    flags = flags | Qt::ItemIsDragEnabled;
    }
  if (this->canBeAParent(node))
    {
    flags = flags | Qt::ItemIsDropEnabled;
    }

  return flags;
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateItemDataFromNode(
  QStandardItem* item, vtkMRMLNode* node, int column)
{
  Q_D(qMRMLIGTLIOModel);
  if (column == this->nameColumn())
    {
    item->setText(QString(node->GetName()));
    item->setToolTip(node->GetNodeTagName());
    }
  if (column == this->toolTipNameColumn())
    {
    item->setToolTip(QString(node->GetName()));
    }
  if (column == this->idColumn())
    {
    item->setText(QString(node->GetID()));
    }
  if (column == this->checkableColumn())
    {
    item->setCheckState(node->GetSelected() ? Qt::Checked : Qt::Unchecked);
    }
  if (column == this->visibilityColumn())
    {
    vtkMRMLDisplayNode* displayNode = vtkMRMLDisplayNode::SafeDownCast(node);
    vtkMRMLDisplayableNode* displayableNode =
      vtkMRMLDisplayableNode::SafeDownCast(node);
    vtkMRMLDisplayableHierarchyNode* displayableHierarchyNode =
      vtkMRMLDisplayableHierarchyNode::SafeDownCast(node);
    if (displayableHierarchyNode)
      {
      displayNode = displayableHierarchyNode->GetDisplayNode();
      }
    int visible = -1;
    if (displayNode)
      {
      visible = displayNode->GetVisibility();
      }
    else if (displayableNode)
      {
      visible = displayableNode->GetDisplayVisibility();
      }
    switch (visible)
      {
      case 0:
        // It should be fine to set the icon even if it is the same, but due
        // to a bug in Qt (http://bugreports.qt.nokia.com/browse/QTBUG-20248),
        // it would fire a superflous itemChanged() signal.
        if (item->icon().cacheKey() != d->HiddenIcon.cacheKey())
          {
          item->setIcon(d->HiddenIcon);
          }
        break;
      case 1:
        // It should be fine to set the icon even if it is the same, but due
        // to a bug in Qt (http://bugreports.qt.nokia.com/browse/QTBUG-20248),
        // it would fire a superflous itemChanged() signal.
        if (item->icon().cacheKey() != d->VisibleIcon.cacheKey())
          {
          item->setIcon(d->VisibleIcon);
          }
        break;
      case 2:
        // It should be fine to set the icon even if it is the same, but due
        // to a bug in Qt (http://bugreports.qt.nokia.com/browse/QTBUG-20248),
        // it would fire a superflous itemChanged() signal.
        if (item->icon().cacheKey() != d->PartiallyVisibleIcon.cacheKey())
          {
          item->setIcon(d->PartiallyVisibleIcon);
          }
        break;
      default:
        break;
      }
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::updateNodeFromItem(vtkMRMLNode* node, QStandardItem* item)
{
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
void qMRMLIGTLIOModel::updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item)
{
  Q_D(qMRMLIGTLIOModel);
  if (item->column() == this->nameColumn())
    {
    node->SetName(item->text().toLatin1());
    }
  // ToolTip can't be edited, don't change the node
  // if (item->column() == this->toolTipNameColumn())
  // {
  // }
  if (item->column() == this->idColumn())
    {
    // Too dangerous
    //node->SetName(item->text().toLatin1());
    }
  if (item->column() == this->checkableColumn())
    {
    node->SetSelected(item->checkState() == Qt::Checked ? 1 : 0);
    }
  if (item->column() == this->visibilityColumn())
    {
    vtkMRMLDisplayNode* displayNode = vtkMRMLDisplayNode::SafeDownCast(node);
    vtkMRMLDisplayableNode* displayableNode =
      vtkMRMLDisplayableNode::SafeDownCast(node);
    vtkMRMLDisplayableHierarchyNode* displayableHierarchyNode =
      vtkMRMLDisplayableHierarchyNode::SafeDownCast(node);
    if (displayableHierarchyNode)
      {
      displayNode = displayableHierarchyNode->GetDisplayNode();
      }
    int visible = -1;
    if (item->icon().cacheKey() == d->HiddenIcon.cacheKey())
      {
      visible = 0;
      }
    else if (item->icon().cacheKey() == d->VisibleIcon.cacheKey())
      {
      visible = 1;
      }
    else if (item->icon().cacheKey() == d->PartiallyVisibleIcon.cacheKey())
      {
      visible = 2;
      }
    if (displayNode)
      {
      displayNode->SetVisibility(visible);
      }
    else if (displayableNode)
      {
      displayableNode->SetDisplayVisibility(visible);
      }
    }
}

//-----------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneEvent(vtkObject* vtk_obj, unsigned long event,
                                        void* client_data, void* call_data)
{
  vtkMRMLScene* scene = reinterpret_cast<vtkMRMLScene*>(vtk_obj);
  qMRMLIGTLIOModel* sceneModel = reinterpret_cast<qMRMLIGTLIOModel*>(client_data);
  vtkMRMLNode* node = reinterpret_cast<vtkMRMLNode*>(call_data);
  Q_ASSERT(scene);
  Q_ASSERT(sceneModel);
  switch(event)
    {
    case vtkMRMLScene::NodeAboutToBeAddedEvent:
      Q_ASSERT(node);
      sceneModel->onMRMLSceneNodeAboutToBeAdded(scene, node);
      break;
    case vtkMRMLScene::NodeAddedEvent:
      Q_ASSERT(node);
      sceneModel->onMRMLSceneNodeAdded(scene, node);
      break;
    case vtkMRMLScene::NodeAboutToBeRemovedEvent:
      Q_ASSERT(node);
      sceneModel->onMRMLSceneNodeAboutToBeRemoved(scene, node);
      break;
    case vtkMRMLScene::NodeRemovedEvent:
      Q_ASSERT(node);
      sceneModel->onMRMLSceneNodeRemoved(scene, node);
      break;
    case vtkCommand::DeleteEvent:
      sceneModel->onMRMLSceneDeleted(scene);
      break;
    case vtkMRMLScene::StartCloseEvent:
      sceneModel->onMRMLSceneAboutToBeClosed(scene);
      break;
    case vtkMRMLScene::EndCloseEvent:
      sceneModel->onMRMLSceneClosed(scene);
      break;
    case vtkMRMLScene::StartImportEvent:
      sceneModel->onMRMLSceneAboutToBeImported(scene);
      break;
    case vtkMRMLScene::EndImportEvent:
      sceneModel->onMRMLSceneImported(scene);
      break;
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeAboutToBeAdded(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  Q_UNUSED(scene);
  Q_UNUSED(node);
#ifndef QT_NO_DEBUG
  Q_D(qMRMLIGTLIOModel);
  Q_ASSERT(scene != 0);
  Q_ASSERT(scene == d->MRMLScene);
#endif
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeAdded(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  Q_D(qMRMLIGTLIOModel);
  Q_UNUSED(d);
  Q_UNUSED(scene);
  Q_ASSERT(scene == d->MRMLScene);
  Q_ASSERT(vtkMRMLNode::SafeDownCast(node));

  if (d->LazyUpdate && d->MRMLScene->IsBatchProcessing())
    {
    return;
    }
  this->insertNode(node);
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeAboutToBeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  Q_D(qMRMLIGTLIOModel);
  Q_UNUSED(d);
  Q_UNUSED(scene);
  Q_ASSERT(scene == d->MRMLScene);

  if (d->LazyUpdate && d->MRMLScene->IsBatchProcessing())
    {
    return;
    }

  int connectionsRemoved =
    qvtkDisconnect(node, vtkCommand::ModifiedEvent,
                   this, SLOT(onMRMLNodeModified(vtkObject*)));

  Q_ASSERT_X((!d->ListenNodeModifiedEvent && connectionsRemoved == 0) ||
             (d->ListenNodeModifiedEvent && connectionsRemoved == 1),
             "qMRMLIGTLIOModel::onMRMLSceneNodeAboutToBeRemoved()",
             "A node has been removed from the scene but the scene model has "
             "never been notified it has been added in the first place. Maybe"
             " vtkMRMLScene::AddNodeNoNotify() has been used instead of "
             "vtkMRMLScene::AddNode");
  Q_UNUSED(connectionsRemoved);

  // TODO: can be fasten by browsing the tree only once
  QModelIndexList indexes = this->match(this->mrmlSceneIndex(), qMRMLIGTLIOModel::UIDRole,
                                        QString(node->GetID()), 1,
                                        Qt::MatchExactly | Qt::MatchRecursive);
  if (indexes.count())
    {
    QStandardItem* item = this->itemFromIndex(indexes[0].sibling(indexes[0].row(),0));
    // The children may be lost if not reparented, we ensure they got reparented.
    while (item->rowCount())
      {
      // we need to remove the children from the node to remove because they
      // would be automatically deleted in qMRMLSceneModel::removeRow()
      d->Orphans.push_back(item->takeRow(0));
      }
    // Remove the item from any orphan list if it exist as we don't want to
    // add it back later in onMRMLSceneNodeRemoved
    foreach(QList<QStandardItem*> orphans, d->Orphans)
      {
      if (orphans.contains(item))
        {
        d->Orphans.removeAll(orphans);
        }
      }
    this->removeRow(indexes[0].row(), indexes[0].parent());
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneNodeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node)
{
  Q_D(qMRMLIGTLIOModel);
  Q_UNUSED(scene);
  Q_UNUSED(node);
  if (d->LazyUpdate && d->MRMLScene->IsBatchProcessing())
    {
    return;
    }
  // The removed node may had children, if they haven't been updated, they
  // are likely to be lost (not reachable when browsing the model), we need
  // to reparent them.
  foreach(QList<QStandardItem*> orphans, d->Orphans)
    {
    QStandardItem* orphan = orphans[0];
    // Make sure that the orphans have not already been reparented.
    if (orphan->parent())
      {
      // Not sure how it is possible, but if it is, then we might want to
      // review the logic behind.
      Q_ASSERT(orphan->parent() == 0);
      continue;
      }
    vtkMRMLNode* node = this->mrmlNodeFromItem(orphan);
    int newIndex = this->nodeIndex(node);
    QStandardItem* newParentItem = this->itemFromNode(this->parentNode(node));
    if (newParentItem == 0)
      {
      newParentItem = this->mrmlSceneItem();
      }
    Q_ASSERT(newParentItem);
    d->reparentItems(orphans, newIndex, newParentItem);
    }
  d->Orphans.clear();
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneDeleted(vtkObject* scene)
{
  Q_UNUSED(scene);
#ifndef QT_NO_DEBUG
  Q_D(qMRMLIGTLIOModel);
  Q_ASSERT(scene == d->MRMLScene);
#endif
  this->setMRMLScene(0);
}

//------------------------------------------------------------------------------
void printStandardItem(QStandardItem* item, const QString& offset)
{
  if (!item)
    {
    return;
    }
  qDebug() << offset << item << item->index() << item->text()
           << item->data(qMRMLIGTLIOModel::UIDRole).toString() << item->row()
           << item->column() << item->rowCount() << item->columnCount();
  for(int i = 0; i < item->rowCount(); ++i )
    {
    for (int j = 0; j < item->columnCount(); ++j)
      {
      printStandardItem(item->child(i,j), offset + "   ");
      }
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLNodeModified(vtkObject* node)
{
  vtkMRMLNode* modifiedNode = vtkMRMLNode::SafeDownCast(node);
  Q_ASSERT(modifiedNode && modifiedNode->GetScene());
  //Q_ASSERT(modifiedNode->GetScene()->IsNodePresent(modifiedNode));
  QModelIndexList nodeIndexes = this->indexes(modifiedNode);
  Q_ASSERT(nodeIndexes.count());
  //qDebug() << "onMRMLNodeModified" << modifiedNode->GetID() << nodeIndexes;
  for (int i = 0; i < nodeIndexes.size(); ++i)
    {
    QModelIndex index = nodeIndexes[i];
    // The node has been modified because it's part of a drag&drop action
    // (reparenting). so it means qMRMLSceneModel has already reparented
    // the row, no need to update the items again.
    //if (d->DraggedNodes.contains(modifiedNode))
    //  {
    //  continue;
    //  }
    QStandardItem* item = this->itemFromIndex(index);
    int oldRow = item->row();
    QStandardItem* oldParent = item->parent();

    this->updateItemFromNode(item, modifiedNode, item->column());
    // maybe the item has been reparented, then we need to rescan the
    // indexes again as may are wrong.
    if (item->row() != oldRow || item->parent() != oldParent)
      {
      int oldSize = nodeIndexes.size();
      nodeIndexes = this->indexes(modifiedNode);
      int newSize = nodeIndexes.size();
      //the number of columns shouldn't change
      Q_ASSERT(oldSize == newSize);
      Q_UNUSED(oldSize);
      Q_UNUSED(newSize);
      }
    }
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onItemChanged(QStandardItem * item)
{
  Q_D(qMRMLIGTLIOModel);

  if (d->PendingItemModified >= 0)
    {
    ++d->PendingItemModified;
    return;
    }
  // when a dnd occurs, the order of the items called with onItemChanged is
  // random, it could be the item in column 1 then the item in column 0
  //qDebug() << "onItemChanged: " << item << item->row() << item->column() << d->DraggedNodes.count();
  //printStandardItem(this->mrmlSceneItem(), "");
  //return;
  // check on the column is optional(no strong feeling), it is just there to be
  // faster though
  if (!this->isANode(item))
    {
    return;
    }

  if (d->DraggedNodes.count())
    {
    if (item->column() == 0)
      {
      //this->metaObject()->invokeMethod(
      //  this, "onItemChanged", Qt::QueuedConnection, Q_ARG(QStandardItem*, item));
      d->DraggedItem = item;
      QTimer::singleShot(100, this, SLOT(delayedItemChanged()));
      }
    return;
    }

  // Only nodes can be changed, scene and extra items should be not editable
  vtkMRMLNode* mrmlNode = this->mrmlNodeFromItem(item);
  Q_ASSERT(mrmlNode);
  this->updateNodeFromItem(mrmlNode, item);
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::delayedItemChanged()
{
  Q_D(qMRMLIGTLIOModel);
  this->onItemChanged(d->DraggedItem);
  d->DraggedItem = 0;
}

//------------------------------------------------------------------------------
bool qMRMLIGTLIOModel::isANode(const QStandardItem * item)const
{
  Q_D(const qMRMLIGTLIOModel);
  return item
    && item != this->mrmlSceneItem()
    && !d->isExtraItem(item);
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneAboutToBeImported(vtkMRMLScene* scene)
{
  Q_UNUSED(scene);
  //this->beginResetModel();
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneImported(vtkMRMLScene* scene)
{
  Q_D(qMRMLIGTLIOModel);
  Q_UNUSED(scene);
  if (d->LazyUpdate)
    {
    this->updateScene();
    }
  //this->endResetModel();
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneAboutToBeClosed(vtkMRMLScene* scene)
{
  Q_UNUSED(scene);
  //this->beginResetModel();
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOModel::onMRMLSceneClosed(vtkMRMLScene* scene)
{
  Q_D(qMRMLIGTLIOModel);
  Q_UNUSED(scene);
  //this->endResetModel();
  if (d->LazyUpdate)
    {
    this->updateScene();
    }
}

//------------------------------------------------------------------------------
Qt::DropActions qMRMLIGTLIOModel::supportedDropActions()const
{
  return Qt::IgnoreAction;
}
