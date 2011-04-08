/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
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
#include <QAction>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMessageBox>

// CTK includes
//#include "ctkModelTester.h"

// qMRML includes
#include "qMRMLSceneModel.h"
#include "qMRMLSceneDisplayableModel.h"
#include "qMRMLSceneModelHierarchyModel.h"
#include "qMRMLSceneTransformModel.h"
#include "qMRMLSortFilterModelHierarchyProxyModel.h"
#include "qMRMLSortFilterProxyModel.h"
#include "qMRMLIGTLConnectorTreeWidget.h"

// MRML includes
#include <vtkMRMLScene.h>

//------------------------------------------------------------------------------
class qMRMLIGTLConnectorTreeWidgetPrivate
{
  Q_DECLARE_PUBLIC(qMRMLIGTLConnectorTreeWidget);
protected:
  qMRMLIGTLConnectorTreeWidget* const q_ptr;
public:
  qMRMLIGTLConnectorTreeWidgetPrivate(qMRMLIGTLConnectorTreeWidget& object);
  void init();
  void setSceneModel(qMRMLSceneModel* newModel);
  void setSortFilterProxyModel(qMRMLSortFilterProxyModel* newSortModel);
  QSize computeSizeHint()const;

  qMRMLSceneModel*           SceneModel;
  qMRMLSortFilterProxyModel* SortFilterModel;
  QString                    SceneModelType;
  bool                       FitSizeToVisibleIndexes;
  
  QMenu*                     NodeMenu;
  vtkMRMLNode*               CurrentNode;
};

//------------------------------------------------------------------------------
qMRMLIGTLConnectorTreeWidgetPrivate::qMRMLIGTLConnectorTreeWidgetPrivate(qMRMLIGTLConnectorTreeWidget& object)
  : q_ptr(&object)
{
  this->SceneModel = 0;
  this->SortFilterModel = 0;
  this->FitSizeToVisibleIndexes = true;
  this->NodeMenu = 0;
  this->CurrentNode = 0;
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidgetPrivate::init()
{
  Q_Q(qMRMLIGTLConnectorTreeWidget);
  this->setSortFilterProxyModel(new qMRMLSortFilterProxyModel(q));
  q->setSceneModelType("Transform");
  
  //ctkModelTester * tester = new ctkModelTester(p);
  //tester->setModel(this->SortFilterModel);
  //QObject::connect(q, SIGNAL(activated(const QModelIndex&)),
  //                 q, SLOT(onActivated(const QModelIndex&)));
  //QObject::connect(q, SIGNAL(clicked(const QModelIndex&)),
  //                 q, SLOT(onActivated(const QModelIndex&)));

  q->setUniformRowHeights(true);
  
  QObject::connect(q, SIGNAL(collapsed(const QModelIndex&)),
                   q, SLOT(onNumberOfVisibleIndexChanged()));
  QObject::connect(q, SIGNAL(expanded(const QModelIndex&)),
                   q, SLOT(onNumberOfVisibleIndexChanged()));
  
  this->NodeMenu = new QMenu(q);
  QAction* deleteAction = new QAction("Delete",this->NodeMenu);
  this->NodeMenu->addAction(deleteAction);
  QObject::connect(deleteAction, SIGNAL(triggered()),
                   q, SLOT(deleteCurrentNode()));
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidgetPrivate::setSceneModel(qMRMLSceneModel* newModel)
{
  Q_Q(qMRMLIGTLConnectorTreeWidget);
  if (!newModel)
    {
    return;
    }

  newModel->setMRMLScene(q->mrmlScene());

  this->SceneModel = newModel;
  this->SortFilterModel->setSourceModel(this->SceneModel);

  q->expandToDepth(2);
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidgetPrivate::setSortFilterProxyModel(qMRMLSortFilterProxyModel* newSortModel)
{
  Q_Q(qMRMLIGTLConnectorTreeWidget);
  if (newSortModel == this->SortFilterModel)
    {
    return;
    }
  
  // delete the previous filter
  delete this->SortFilterModel;
  this->SortFilterModel = newSortModel;
  // Set the input of the view
  // if no filter is given then let's show the scene model directly
  q->QTreeView::setModel(this->SortFilterModel
    ? static_cast<QAbstractItemModel*>(this->SortFilterModel)
    : static_cast<QAbstractItemModel*>(this->SceneModel));
  // Setting a new model to the view resets the selection model
  QObject::connect(q->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
                   q, SLOT(onCurrentRowChanged(const QModelIndex&)));
  if (!this->SortFilterModel)
    {
    return;
    }
  this->SortFilterModel->setParent(q);
  // Set the input of the filter
  this->SortFilterModel->setSourceModel(this->SceneModel);

  // resize the view if new rows are added/removed
  QObject::connect(this->SortFilterModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
                   q, SLOT(onNumberOfVisibleIndexChanged()));
  QObject::connect(this->SortFilterModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                   q, SLOT(onNumberOfVisibleIndexChanged()));

  q->expandToDepth(2);
  q->onNumberOfVisibleIndexChanged();
}

//------------------------------------------------------------------------------
QSize qMRMLIGTLConnectorTreeWidgetPrivate::computeSizeHint()const
{
  Q_Q(const qMRMLIGTLConnectorTreeWidget);
  int visibleIndexCount = 0;
  for(QModelIndex index = this->SortFilterModel->mrmlSceneIndex();
      index.isValid();
      index = q->indexBelow(index))
    {
    ++visibleIndexCount;
    }

  QSize treeViewSizeHint = q->QTreeView::sizeHint();
  treeViewSizeHint.setHeight(
    q->frameWidth()
    + (q->isHeaderHidden() ? 0 : q->header()->sizeHint().height())
    + visibleIndexCount * q->sizeHintForRow(0)
    + (q->horizontalScrollBar()->isVisibleTo(const_cast<qMRMLIGTLConnectorTreeWidget*>(q)) ? q->horizontalScrollBar()->height() : 0)
    + q->frameWidth());
  return treeViewSizeHint;
}

//------------------------------------------------------------------------------
// qMRMLIGTLConnectorTreeWidget
//------------------------------------------------------------------------------
qMRMLIGTLConnectorTreeWidget::qMRMLIGTLConnectorTreeWidget(QWidget *_parent)
  :QTreeView(_parent)
  , d_ptr(new qMRMLIGTLConnectorTreeWidgetPrivate(*this))
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  d->init();
}

//------------------------------------------------------------------------------
qMRMLIGTLConnectorTreeWidget::~qMRMLIGTLConnectorTreeWidget()
{
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  Q_ASSERT(d->SortFilterModel);
  // only qMRMLSceneModel needs the scene, the other proxies don't care.
  d->SceneModel->setMRMLScene(scene);
  this->expandToDepth(2);
}

//------------------------------------------------------------------------------
QString qMRMLIGTLConnectorTreeWidget::sceneModelType()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  return d->SceneModelType;
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setSceneModelType(const QString& modelName)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);

  qMRMLSceneModel* newModel = 0;
  qMRMLSortFilterProxyModel* newFilterModel = d->SortFilterModel;
  // switch on the incoming model name
  if (modelName == QString("Transform"))
    {
    newModel = new qMRMLSceneTransformModel(this);
    }
  else if (modelName == QString("Displayable"))
    {
    newModel = new qMRMLSceneDisplayableModel(this);
    }
  else if (modelName == QString("ModelHierarchy"))
    {
    newModel = new qMRMLSceneModelHierarchyModel(this);
    newFilterModel = new qMRMLSortFilterModelHierarchyProxyModel(this);
    }
  if (newModel)
    {
    d->SceneModelType = modelName;
    newModel->setListenNodeModifiedEvent(this->listenNodeModifiedEvent());
    }
  if (newFilterModel)
    {
    newFilterModel->setNodeTypes(this->nodeTypes());
    }
  d->setSceneModel(newModel);
  // typically a no op except for ModelHierarchy
  d->setSortFilterProxyModel(newFilterModel);
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setSceneModel(qMRMLSceneModel* newSceneModel, const QString& modelType)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);

  if (!newSceneModel) 
    {
    return;
    }
  d->SceneModelType = modelType;
  d->setSceneModel(newSceneModel);
}

//------------------------------------------------------------------------------
vtkMRMLScene* qMRMLIGTLConnectorTreeWidget::mrmlScene()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  return d->SceneModel ? d->SceneModel->mrmlScene() : 0;
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::onCurrentRowChanged(const QModelIndex& index)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  Q_ASSERT(d->SortFilterModel);
  emit currentNodeChanged(d->SortFilterModel->mrmlNodeFromIndex(index));
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setListenNodeModifiedEvent(bool listen)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  Q_ASSERT(d->SceneModel);
  d->SceneModel->setListenNodeModifiedEvent(listen);
}

//------------------------------------------------------------------------------
bool qMRMLIGTLConnectorTreeWidget::listenNodeModifiedEvent()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  return d->SceneModel ? d->SceneModel->listenNodeModifiedEvent() : false;
}

// --------------------------------------------------------------------------
QStringList qMRMLIGTLConnectorTreeWidget::nodeTypes()const
{
  return this->sortFilterProxyModel()->nodeTypes();
}

// --------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setNodeTypes(const QStringList& _nodeTypes)
{
  this->sortFilterProxyModel()->setNodeTypes(_nodeTypes);
}

//--------------------------------------------------------------------------
qMRMLSortFilterProxyModel* qMRMLIGTLConnectorTreeWidget::sortFilterProxyModel()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  Q_ASSERT(d->SortFilterModel);
  return d->SortFilterModel;
}

//--------------------------------------------------------------------------
qMRMLSceneModel* qMRMLIGTLConnectorTreeWidget::sceneModel()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  Q_ASSERT(d->SceneModel);
  return d->SceneModel;
}

//--------------------------------------------------------------------------
QSize qMRMLIGTLConnectorTreeWidget::minimumSizeHint()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  QSize minSize = this->QTreeView::minimumSizeHint();
  return minSize.expandedTo(d->computeSizeHint());
}

//--------------------------------------------------------------------------
QSize qMRMLIGTLConnectorTreeWidget::sizeHint()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  QSize size = this->QTreeView::minimumSizeHint();
  return size.expandedTo(d->computeSizeHint());
}

//--------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::updateGeometries()
{
  // don't update the geometries if it's not visible on screen
  // UpdateGeometries is for tree child widgets geometry
  if (!this->isVisible())
    {
    return;
    }
  this->QTreeView::updateGeometries();
}

//--------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::onNumberOfVisibleIndexChanged()
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  if (d->FitSizeToVisibleIndexes)
    {
    this->updateGeometry();
    }
}

//--------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::setFitSizeToVisibleIndexes(bool enable)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  d->FitSizeToVisibleIndexes = enable;
  if (d->FitSizeToVisibleIndexes)
    {
    this->updateGeometry();
    }
}

//--------------------------------------------------------------------------
bool qMRMLIGTLConnectorTreeWidget::fitSizeToVisibleIndexes()const
{
  Q_D(const qMRMLIGTLConnectorTreeWidget);
  return d->FitSizeToVisibleIndexes;
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::mousePressEvent(QMouseEvent* e)
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  this->QTreeView::mousePressEvent(e);
  
  if (e->button() != Qt::RightButton)
    {
    return;
    }
  // get the index of the current column
  QModelIndex index = this->indexAt(e->pos());
  
  vtkMRMLNode* node = this->sortFilterProxyModel()->mrmlNodeFromIndex(index);
  
  if (!node)
    {
    return;
    }
  
  // Don't support annotation deletion yet
  if (node->IsA("vtkMRMLAnnotationHierarchyNode") || 
      node->IsA("vtkMRMLAnnotationNode") ||
      node->IsA("vtkMRMLAnnotationDisplayNode"))
    {
    return;
    }

  d->CurrentNode = node;
  d->NodeMenu->exec(e->globalPos());
  d->CurrentNode = 0;
}

//------------------------------------------------------------------------------
void qMRMLIGTLConnectorTreeWidget::deleteCurrentNode()
{
  Q_D(qMRMLIGTLConnectorTreeWidget);
  if (!d->CurrentNode)
    {
    qWarning() << "No node to delete";
    return;
    }
  this->mrmlScene()->RemoveNode(d->CurrentNode);
  d->CurrentNode = 0;
}
