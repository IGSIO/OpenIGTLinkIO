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
#include <QMouseEvent>
#include <QMessageBox>
#include <QHeaderView>

// CTK includes
#include "ctkModelTester.h"

// qMRML includes
#include "qMRMLSceneModel.h"
#include "qMRMLSortFilterProxyModel.h"
#include "qMRMLSceneTransformModel.h"
#include "qMRMLIGTLIOModel.h"
#include "qMRMLTreeView.h"

// OpenIGTLinkIF GUI includes
#include "qMRMLIGTLIOTreeView.h"

// OpenIGTLinkIF Logic includes
#include "vtkSlicerOpenIGTLinkIFLogic.h"

// MRML includes
#include <vtkMRMLNode.h>

//------------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class qMRMLIGTLIOTreeViewPrivate
{
  Q_DECLARE_PUBLIC(qMRMLIGTLIOTreeView);
protected:
  qMRMLIGTLIOTreeView* const q_ptr;
public:
  qMRMLIGTLIOTreeViewPrivate(qMRMLIGTLIOTreeView& object);
  void init();

  qMRMLIGTLIOModel*                 SceneModel;
  qMRMLSortFilterProxyModel*        SortFilterModel;
  vtkSlicerOpenIGTLinkIFLogic*      Logic;
};

//------------------------------------------------------------------------------
qMRMLIGTLIOTreeViewPrivate::qMRMLIGTLIOTreeViewPrivate(qMRMLIGTLIOTreeView& object)
  : q_ptr(&object)
{
  this->SceneModel = 0;
  this->SortFilterModel = 0;
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOTreeViewPrivate::init()
{
  Q_Q(qMRMLIGTLIOTreeView);

  this->SceneModel = new qMRMLIGTLIOModel(q);
  q->setSceneModel(this->SceneModel, "IGTLConnector");

  QStringList nodeTypes = QStringList();
  nodeTypes.append("vtkMRMLIGTLConnectorNode");

  q->setNodeTypes(nodeTypes);
  this->SortFilterModel = q->sortFilterProxyModel();

  q->setUniformRowHeights(true);

  q->expandAll();
}

//------------------------------------------------------------------------------
qMRMLIGTLIOTreeView::qMRMLIGTLIOTreeView(QWidget *_parent)
  : Superclass(_parent)
  , d_ptr(new qMRMLIGTLIOTreeViewPrivate(*this))
{
  Q_D(qMRMLIGTLIOTreeView);
  d->init();

  // we need to enable mouse tracking to set the appropriate cursor while mouseMove occurs
  this->setMouseTracking(true);
}

//------------------------------------------------------------------------------
qMRMLIGTLIOTreeView::~qMRMLIGTLIOTreeView()
{
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOTreeView::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLIGTLIOTreeView);
  Q_ASSERT(d->SortFilterModel);
  // only qMRMLSceneModel needs the scene, the other proxies don't care.
  d->SceneModel->setMRMLScene(scene);

  this->expandAll();
}

//------------------------------------------------------------------------------
// Click and selected event handling
//------------------------------------------------------------------------------


void qMRMLIGTLIOTreeView::onCurrentRowChanged(const QModelIndex& index)
{
  Q_D(qMRMLIGTLIOTreeView);
  Q_ASSERT(d->SortFilterModel);
  Q_ASSERT(this->currentNode() == d->SortFilterModel->mrmlNodeFromIndex(index));

  vtkMRMLNode* node = d->SortFilterModel->mrmlNodeFromIndex(index);
  if (node)
    {
    emit selectable(false);
    emit addable(false);
    emit removable(false);
    emit currentNodeChanged(node);
    }
  else
    {
    emit selectable(true);
    emit addable(true);
    emit removable(true);
    emit currentNodeChanged(NULL);
    }
}



//------------------------------------------------------------------------------
//
// MouseMove event handling
//
//------------------------------------------------------------------------------

#ifndef QT_NO_CURSOR
//------------------------------------------------------------------------------
bool qMRMLIGTLIOTreeView::viewportEvent(QEvent* e)
{

  // reset the cursor if we leave the viewport
  if(e->type() == QEvent::Leave)
    {
    setCursor(QCursor());
    }

  return QTreeView::viewportEvent(e);
}

//------------------------------------------------------------------------------
void qMRMLIGTLIOTreeView::mouseMoveEvent(QMouseEvent* e)
{
  this->QTreeView::mouseMoveEvent(e);

  // get the index of the current column
  QModelIndex index = indexAt(e->pos());

  //if (index.column() == qMRMLSceneIGTLConnectorModel::VisibilityColumn || index.column() == qMRMLSceneIGTLConnectorModel::LockColumn || index.column() == qMRMLSceneIGTLConnectorModel::EditColumn)
  //  {
  //  // we are over a column with a clickable icon
  //  // let's change the cursor
  //  QCursor handCursor(Qt::PointingHandCursor);
  //  this->setCursor(handCursor);
  //  // .. and bail out
  //  return;
  //  }
  //else if(this->cursor().shape() == Qt::PointingHandCursor)
  //  {
  //  // if we are NOT over such a column and we already have a changed cursor,
  //  // reset it!
  //  this->setCursor(QCursor());
  //  }

}
#endif

//------------------------------------------------------------------------------
//
// Layout and behavior customization
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void qMRMLIGTLIOTreeView::setSelectedNode(const char* id)
{
  Q_D(qMRMLIGTLIOTreeView);

  vtkMRMLNode* node = this->mrmlScene()->GetNodeByID(id);

  if (node)
    {
    this->setCurrentIndex(d->SortFilterModel->indexFromMRMLNode(node));
    }
}

//------------------------------------------------------------------------------
//
// In-Place Editing of Annotations
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//void qMRMLIGTLIOTreeView::onVisibilityColumnClicked(vtkMRMLNode* node)
//{
//  if (!node)
//    {
//    // no node found!
//    return;
//    }

//  //vtkMRMLAnnotationNode* annotationNode = vtkMRMLAnnotationNode::SafeDownCast(node);

//  //if (annotationNode)
//  //  {
//  //  // this is a valid annotationNode
//  //  annotationNode->SetVisible(!annotationNode->GetVisible());
//  //
//  //  }


//  // TODO move to logic
//  //vtkMRMLAnnotationHierarchyNode* hierarchyNode = vtkMRMLAnnotationHierarchyNode::SafeDownCast(node);
//  //
//  //if (hierarchyNode)
//  //  {
//  //  vtkCollection* children = vtkCollection::New();
//  //  hierarchyNode->GetChildrenDisplayableNodes(children);
//  //
//  //  children->InitTraversal();
//  //  for (int i=0; i<children->GetNumberOfItems(); ++i)
//  //    {
//  //    vtkMRMLAnnotationNode* childNode = vtkMRMLAnnotationNode::SafeDownCast(children->GetItemAsObject(i));
//  //    if (childNode)
//  //      {
//  //      // this is a valid annotation child node
//  //      //
//  //      childNode->SetVisible(!childNode->GetVisible());
//  //      }
//  //    } // for loop
//  //
//  //  } // if hierarchyNode
//}

//------------------------------------------------------------------------------
//void qMRMLIGTLIOTreeView::onLockColumnClicked(vtkMRMLNode* node)
//{

//  if (!node)
//    {
//    // no node found!
//    return;
//    }

//  //vtkMRMLAnnotationNode* annotationNode = vtkMRMLAnnotationNode::SafeDownCast(node);
//  //
//  //if (annotationNode)
//  //  {
//  //  // this is a valid annotationNode
//  //  annotationNode->SetLocked(!annotationNode->GetLocked());
//  //
//  //  }


//  // TODO move to logic
//  //vtkMRMLAnnotationHierarchyNode* hierarchyNode = vtkMRMLAnnotationHierarchyNode::SafeDownCast(node);
//  //
//  //if (hierarchyNode)
//  //  {
//  //  vtkCollection* children = vtkCollection::New();
//  //  hierarchyNode->GetChildrenDisplayableNodes(children);
//  //
//  //  children->InitTraversal();
//  //  for (int i=0; i<children->GetNumberOfItems(); ++i)
//  //    {
//  //    vtkMRMLAnnotationNode* childNode = vtkMRMLAnnotationNode::SafeDownCast(children->GetItemAsObject(i));
//  //    if (childNode)
//  //      {
//  //      // this is a valid annotation child node
//  //      //
//  //      childNode->SetLocked(!childNode->GetLocked());
//  //      }
//  //    } // for loop
//  //
//  //  } // if hierarchyNode
//}

//------------------------------------------------------------------------------
void qMRMLIGTLIOTreeView::mousePressEvent(QMouseEvent* event)
{
  // skip qMRMLTreeView
  this->QTreeView::mousePressEvent(event);
}

//------------------------------------------------------------------------------
//
// Connections to other classes
//
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Set and observe the logic
//-----------------------------------------------------------------------------
void qMRMLIGTLIOTreeView::setLogic(vtkSlicerOpenIGTLinkIFLogic* logic)
{
  Q_D(qMRMLIGTLIOTreeView);
  if (!logic)
    {
    return;
    }

  d->Logic = logic;

  // propagate down to model
  d->SceneModel->setLogic(d->Logic);
}
