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
#include "igtlioLogic.h"
#include "igtlioConnector.h"

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

  igtlioConnector* cnode = this->getNodeFromIndex(index);

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
      Q_ASSERT(cnode->GetType() < igtlioConnector::NUM_TYPE);
      return QString::fromStdString(igtlioConnector::ConnectorTypeStr[cnode->GetType()]);
      break;
      }
    case qIGTLIOConnectorModel::StatusColumn:
      {
      Q_ASSERT(cnode->GetState() < igtlioConnector::NUM_STATE);
      return QString::fromStdString(igtlioConnector::ConnectorStateStr[cnode->GetState()]);
      break;
      }
    case qIGTLIOConnectorModel::HostnameColumn:
      {
      if (cnode->GetType() == igtlioConnector::TYPE_CLIENT)
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
  if (!Logic)
  {
    return QModelIndex();
  }
  if (row >= Logic->GetNumberOfConnectors())
  {
    return QModelIndex();
  }
  return createIndex(row, column, Logic->GetConnector(row));
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
void qIGTLIOConnectorModel::setLogic(igtlioLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << igtlioLogic::ConnectionAddedEvent
          << igtlioLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionEvent(vtkObject*, unsigned long, void*, void* )));
    }

  this->Logic = logic;
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::ReconnectConnector(igtlioConnector* oldConnector, igtlioConnector* newConnector)
{
  foreach(int evendId, QList<int>()
          << igtlioConnector::ConnectedEvent
          << igtlioConnector::DisconnectedEvent
          << igtlioConnector::ActivatedEvent
          << igtlioConnector::DeactivatedEvent)
    {
    qvtkReconnect(oldConnector, newConnector, evendId,
                  this, SLOT(onConnectorEvent(vtkObject*, unsigned long, void*, void* )));
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::onConnectionEvent(vtkObject* caller, unsigned long event, void * , void* connector )
{
  if (event==igtlioLogic::ConnectionAddedEvent)
    {
      igtlioConnector* c = static_cast<igtlioConnector*>(connector);
      this->ReconnectConnector(NULL, c);
      this->resetModel();
    }
  if (event==igtlioLogic::ConnectionAboutToBeRemovedEvent)
    {
      igtlioConnector* c = static_cast<igtlioConnector*>(connector);
      this->ReconnectConnector(c, NULL);
      this->resetModel();
    }
}

//-----------------------------------------------------------------------------
void qIGTLIOConnectorModel::onConnectorEvent(vtkObject* caller, unsigned long event , void*, void* connector )
{
  emit dataChanged(QModelIndex(), QModelIndex());
}

//-----------------------------------------------------------------------------
igtlioConnector* qIGTLIOConnectorModel::getNodeFromIndex(const QModelIndex &index) const
{
  if (!index.isValid())
    return NULL;
  return static_cast<igtlioConnector*>(index.internalPointer());
}
