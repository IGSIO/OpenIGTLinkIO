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

#ifndef __qMRMLIGTLIOModel_h
#define __qMRMLIGTLIOModel_h

// Qt includes
#include "qMRMLSceneModel.h"

// qMRML includes
class vtkMRMLNode;
class vtkMRMLScene;
class qMRMLIGTLIOModelPrivate;

//------------------------------------------------------------------------------
class QMRML_WIDGETS_EXPORT qMRMLIGTLIOModel : public qMRMLSceneModel
{
  Q_OBJECT
  QVTK_OBJECT

  ///// Observe (or not) the modified event of the node and update the node item
  ///// data accordingly. It can be useful when the modified property is displayed
  ///// (name, id, visibility...)
  //Q_PROPERTY (bool listenNodeModifiedEvent READ listenNodeModifiedEvent WRITE setListenNodeModifiedEvent)
  //
  ///// Control in which column vtkMRMLNode names are displayed (Qt::DisplayRole).
  ///// A value of -1 hides it. First column (0) by default.
  //Q_PROPERTY (int nameColumn READ nameColumn WRITE setNameColumn)
  ///// Control in which column vtkMRMLNode IDs are displayed (Qt::DisplayRole).
  ///// A value of -1 hides it. Second column (1) by default.
  //Q_PROPERTY (int idColumn READ idColumn WRITE setIDColumn)
  ///// Control in which column vtkMRMLNode::Selected are displayed (Qt::CheckStateRole).
  ///// A value of -1 hides it. Hidden by default (value of -1).
  //Q_PROPERTY (int checkableColumn READ checkableColumn WRITE setCheckableColumn)
  ///// Control in which column vtkMRMLNode::Visibility are displayed (Qt::DecorationRole).
  ///// A value of -1 hides it. Hidden by default (value of -1).
  //Q_PROPERTY (int visibilityColumn READ visibilityColumn WRITE setVisibilityColumn)
  ///// Control in which column tooltips are displayed (Qt::ToolTipRole).
  ///// A value of -1 hides it. Hidden by default (value of -1).
  //Q_PROPERTY (int toolTipNameColumn READ toolTipNameColumn WRITE setToolTipNameColumn)

public:
  typedef qMRMLSceneModel Superclass;
  qMRMLIGTLIOModel(QObject *parent=0);
  virtual ~qMRMLIGTLIOModel();
  
  virtual Qt::DropActions supportedDropActions()const;
  virtual QMimeData* mimeData(const QModelIndexList& indexes)const;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                            int row, int column, const QModelIndex &parent);

  /// Must be reimplemented in derived classes
  /// Returns 0 (scene is not a node) in qMRMLIGTLIOModel
  virtual vtkMRMLNode* parentNode(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes
  virtual int          nodeIndex(vtkMRMLNode* node)const;
  /// fast function that only check the type of the node to know if it can be a child.
  virtual bool         canBeAChild(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes
  /// Returns false in qMRMLIGTLIOModel
  virtual bool         canBeAParent(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes.
  /// Doesn't reparent and returns false by qMRMLIGTLIOModel
  virtual bool         reparent(vtkMRMLNode* node, vtkMRMLNode* newParent);

protected slots:

  virtual void onMRMLSceneNodeAboutToBeAdded(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeAboutToBeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeAdded(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node);

  virtual void onMRMLSceneAboutToBeImported(vtkMRMLScene* scene);
  virtual void onMRMLSceneImported(vtkMRMLScene* scene);
  virtual void onMRMLSceneAboutToBeClosed(vtkMRMLScene* scene);
  virtual void onMRMLSceneClosed(vtkMRMLScene* scene);

  void onMRMLSceneDeleted(vtkObject* scene);

  void onMRMLNodeModified(vtkObject* node);
  void onItemChanged(QStandardItem * item);
  void delayedItemChanged();

protected:

  qMRMLIGTLIOModel(qMRMLIGTLIOModelPrivate* pimpl, QObject *parent=0);

  virtual void updateScene();
  virtual void populateScene();
  virtual QStandardItem* insertNode(vtkMRMLNode* node);
  virtual QStandardItem* insertNode(vtkMRMLNode* node, QStandardItem* parent, int row = -1);

  virtual bool isANode(const QStandardItem* item)const;
  virtual QFlags<Qt::ItemFlag> nodeFlags(vtkMRMLNode* node, int column)const;
  /// Generic function that updates the item data and flags from the node.
  /// You probably want to reimplement updateItemDataFromNode() instead.
  /// \sa updateNodeFromItemData, updateNodeFromItem, updateItemDataFromNode,
  /// nodeFlags
  virtual void updateItemFromNode(QStandardItem* item, vtkMRMLNode* node, int column);
  /// To reimplement if you want custom display of the QStandardItem from
  /// the MRML node.
  /// Example:
  /// <code>
  /// void MySceneModel::updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
  /// if (column == 3)
  ///   {
  ///   item->setText(node->GetFoo());
  ///   return;
  ///   }
  /// this->Superclass::updateItemDataFromNode(item, node, column);
  /// }
  /// </code>
  /// \sa updateNodeFromItemData, updateNodeFromItem, updateItemFromNode,
  /// nodeFlags
  virtual void updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column);
  /// Generic function that updates the node from the item data and flags.
  /// You probably want to reimplement updateNodeFromItemData() instead.
  /// \sa updateItemDataFromNode, updateNodeFromItemData, updateItemFromNode,
  /// nodeFlags
  virtual void updateNodeFromItem(vtkMRMLNode* node, QStandardItem* item);
  /// To reimplement if you want to propagate user changes on QStandardItem
  /// into the MRML node.
  /// Example:
  /// <code>
  /// void MySceneModel::updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item)
  /// if (column == 3)
  ///   {
  ///   node->SetFoo(item->text());
  ///   return;
  ///   }
  /// this->Superclass::updateNodeFromItemData(node, item);
  /// }
  /// </code>
  /// \sa updateItemFromNode, updateNodeFromItemData, updateItemFromNode,
  /// nodeFlags
  virtual void updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item);

  static void onMRMLSceneEvent(vtkObject* vtk_obj, unsigned long event,
                               void* client_data, void* call_data);
protected:
  QScopedPointer<qMRMLIGTLIOModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLIGTLIOModel);
  Q_DISABLE_COPY(qMRMLIGTLIOModel);
};

#endif
