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
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

// IGTLIO includes
#include "qIGTLIOConnectorModel.h"
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"

//------------------------------------------------------------------------------
qIGTLIOConnectorModel::qIGTLIOConnectorModel(QObject *vparent)
  :QAbstractItemModel(vparent)
{
  HeaderLabels = QStringList() << "Name" << "Type" << "Status" << "Hostname" << "Port";
}

//------------------------------------------------------------------------------
qIGTLIOConnectorModel::~qIGTLIOConnectorModel()
{
}

//-----------------------------------------------------------------------------
int qIGTLIOConnectorModel::columnCount(const QModelIndex& parent) const
{
  return HeaderLabels.size();
}

//-----------------------------------------------------------------------------
int qIGTLIOConnectorModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() > 0)
    return 0;

  // only topnode has children
  if (!parent.isValid())
  {
    if (!Logic)
    {
        std::cout << "WARNING in: int qIGTLIOConnectorModel::rowCount(const QModelIndex& parent) const: Logic is a NULL object! Returning 0." << std::endl;
        return 0;
    }
    return Logic->GetNumberOfConnectors();
  }
  return 0;
}

//------------------------------------------------------------------------------
QVariant qIGTLIOConnectorModel::data(const QModelIndex &index, int role) const
{
  if (role!=Qt::DisplayRole)
    return QVariant();

  vtkIGTLIOConnector* cnode = this->getNodeFromIndex(index);

  if (!cnode)
    {
    return QVariant();
    }
  switch (index.column())
    {
    case qIGTLIOConnectorModel::NameColumn:
      {
      return QString::fromStdString(cnode->GetName());
      break;
      }
    case qIGTLIOConnectorModel::TypeColumn:
      {
      Q_ASSERT(cnode->GetType() < vtkIGTLIOConnector::NUM_TYPE);
      return QString::fromStdString(vtkIGTLIOConnector::ConnectorTypeStr[cnode->GetType()]);
      break;
      }
    case qIGTLIOConnectorModel::StatusColumn:
      {
      Q_ASSERT(cnode->GetState() < vtkIGTLIOConnector::NUM_STATE);
      return QString::fromStdString(vtkIGTLIOConnector::ConnectorStateStr[cnode->GetState()]);
      break;
      }
    case qIGTLIOConnectorModel::HostnameColumn:
      {
      if (cnode->GetType() == vtkIGTLIOConnector::TYPE_CLIENT)
        {
        return QString::fromStdString(cnode->GetServerHostname());
        }
      else
        {
        return QString("--");
        }
      break;
      }
    case qIGTLIOConnectorModel::PortColumn:
      {
      return QString("%1").arg(cnode->GetServerPort());
      break;
      }
    default:
      break;
    }
  return QVariant();
}


//-----------------------------------------------------------------------------
Qt::ItemFlags qIGTLIOConnectorModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
QVariant qIGTLIOConnectorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
      return HeaderLabels[section];
    }

  return QVariant();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIOConnectorModel::index(int row, int column, const QModelIndex &parent) const
{
  if (row < Logic->GetNumberOfConnectors())
    {
      return createIndex(row, column, Logic->GetConnector(row));
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIOConnectorModel::parent(const QModelIndex &index) const
{
  return QModelIndex();
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::resetModel()
{
  this->beginResetModel();
  this->endResetModel();
}


//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::setLogic(vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOLogic::ConnectionAddedEvent
          << vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionEvent(vtkObject*, void*, unsigned long, void*)));
    }

  this->Logic = logic;
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::ReconnectConnector(vtkIGTLIOConnector* oldConnector, vtkIGTLIOConnector* newConnector)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOConnector::ConnectedEvent
          << vtkIGTLIOConnector::DisconnectedEvent
          << vtkIGTLIOConnector::ActivatedEvent
          << vtkIGTLIOConnector::DeactivatedEvent)
    {
    qvtkReconnect(oldConnector, newConnector, evendId,
                  this, SLOT(onConnectorEvent(vtkObject*, void*, unsigned long, void*)));
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::onConnectionEvent(vtkObject* caller, void* connector, unsigned long event , void*)
{
  if (event==vtkIGTLIOLogic::ConnectionAddedEvent)
    {
//      std::cout << "on add connected event" << std::endl;
      vtkIGTLIOConnector* c = static_cast<vtkIGTLIOConnector*>(connector);
      this->ReconnectConnector(NULL, c);
      this->resetModel();
    }
  if (event==vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
//      std::cout << "on remove connected event" << std::endl;
      vtkIGTLIOConnector* c = static_cast<vtkIGTLIOConnector*>(connector);
      this->ReconnectConnector(c, NULL);
      this->resetModel();
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::onConnectorEvent(vtkObject* caller, void* connector, unsigned long event , void*)
{
  emit dataChanged(QModelIndex(), QModelIndex());
}

//-----------------------------------------------------------------------------
vtkIGTLIOConnector* qIGTLIOConnectorModel::getNodeFromIndex(const QModelIndex &index) const
{
  if (!index.isValid())
    return NULL;
  return static_cast<vtkIGTLIOConnector*>(index.internalPointer());
}
