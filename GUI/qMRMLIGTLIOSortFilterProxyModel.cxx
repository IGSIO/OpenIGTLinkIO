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

// qMRML includes
#include "qMRMLIGTLIOModel.h"
#include "qMRMLIGTLIOSortFilterProxyModel.h"

// VTK includes
#include <vtkMRMLNode.h>

#include "vtkMRMLIGTLConnectorNode.h"

// -----------------------------------------------------------------------------
// qMRMLIGTLIOSortFilterProxyModelPrivate

// -----------------------------------------------------------------------------
class qMRMLIGTLIOSortFilterProxyModelPrivate
{
public:
  qMRMLIGTLIOSortFilterProxyModelPrivate();
};

// -----------------------------------------------------------------------------
qMRMLIGTLIOSortFilterProxyModelPrivate::qMRMLIGTLIOSortFilterProxyModelPrivate()
{
}

// -----------------------------------------------------------------------------
// qMRMLIGTLIOSortFilterProxyModel

//------------------------------------------------------------------------------
qMRMLIGTLIOSortFilterProxyModel::qMRMLIGTLIOSortFilterProxyModel(QObject *vparent)
  : qMRMLSortFilterProxyModel(vparent)
  , d_ptr(new qMRMLIGTLIOSortFilterProxyModelPrivate)
{
}

//------------------------------------------------------------------------------
qMRMLIGTLIOSortFilterProxyModel::~qMRMLIGTLIOSortFilterProxyModel()
{
}

//------------------------------------------------------------------------------
vtkMRMLIGTLConnectorNode* qMRMLIGTLIOSortFilterProxyModel::parentConnector(const QModelIndex& index)
{
  Q_D(qMRMLIGTLIOSortFilterProxyModel);
  
  qMRMLSceneModel* sceneModel = qobject_cast<qMRMLSceneModel*>(this->sourceModel());
  QStandardItem* item = sceneModel->itemFromIndex(index);
  if (!item)
    {
    return NULL;
    }

  QStandardItem* parentIO = item->parent();
  if (!parentIO)
    {
    return NULL;
    }

  QStandardItem* parentConnector = parentIO->parent();
  if (!parentConnector)
    {
    return NULL;
    }

  vtkMRMLNode* node = this->mrmlNodeFromIndex(parentConnector->index());
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);

  return cnode;

}


//------------------------------------------------------------------------------
int qMRMLIGTLIOSortFilterProxyModel::parentConnectorDirection(const QModelIndex& index)
{
  Q_D(qMRMLIGTLIOSortFilterProxyModel);

  qMRMLSceneModel* sceneModel = qobject_cast<qMRMLSceneModel*>(this->sourceModel());
  QStandardItem* item = sceneModel->itemFromIndex(index);
  if (!item)
    {
    return 0;
    }

  QStandardItem* parentIO = item->parent();
  if (!parentIO)
    {
    return 0;
    }

  // Check if the parent node is actually a connector node
  QStandardItem* parentConnector = parentIO->parent();
  if (!parentConnector)
    {
    return 0;
    }

  vtkMRMLNode* node = this->mrmlNodeFromIndex(parentConnector->index());
  vtkMRMLIGTLConnectorNode * cnode = vtkMRMLIGTLConnectorNode::SafeDownCast(node);

  if (!cnode)
    {
    return 0;
    }

  QString text = parentIO->child(0,0)->text();
  if (text.compare(QString("IN")) == 0)
    {
    return 1;
    }
  else
    {
    return 2;
    }
}


