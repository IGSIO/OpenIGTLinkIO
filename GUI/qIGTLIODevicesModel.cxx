#include "qIGTLIODevicesModel.h"

// Qt includes
#include <QDebug>
#include <QMap>
#include <QMimeData>
#include <QSharedPointer>
#include <QStack>
#include <QStringList>
#include <QVector>
#include <QItemSelectionModel>

// OpenIGTLinkIF GUI includes
#include <qIGTLIODevicesModel.h>

#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkIGTLIONode.h"

// VTK includes
#include <vtkVariantArray.h>
#include "qIGTLIODevicesModel.h"
#include <vtkEventQtSlotConnect.h>

//------------------------------------------------------------------------------
qIGTLIODevicesModel::qIGTLIODevicesModel(QObject *vparent)
  :QAbstractItemModel(vparent)
{
  HeaderLabels = QStringList() << "Name" << "MRML Type" << "IGTL Type" << "Vis" << "Push on Connect";
}

//------------------------------------------------------------------------------
qIGTLIODevicesModel::~qIGTLIODevicesModel()
{
}

//-----------------------------------------------------------------------------
int qIGTLIODevicesModel::columnCount(const QModelIndex& parent) const
{
  return HeaderLabels.size();
}

void dmsg(QString msg)
{
}

//-----------------------------------------------------------------------------
int qIGTLIODevicesModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() > 0)
    return 0;

  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(parent);
  if(!node)
  {
      std::cout << "WARNING in: int qIGTLIODevicesModel::rowCount(const QModelIndex& parent) const: node is a NULL object! Returning 0." << std::endl;
      return 0;
  }

  int r = node->GetNumberOfChildren();
  return r;
}

//------------------------------------------------------------------------------
QVariant qIGTLIODevicesModel::data(const QModelIndex &index, int role) const
{
  if (role!=Qt::DisplayRole && role!=Qt::CheckStateRole)
    return QVariant();

  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(index);

  if (!node)
    {
      return QVariant();
    }

  if (role==Qt::CheckStateRole)
    {
      switch (index.column())
        {
        case qIGTLIODevicesModel::PushOnConnectColumn:
          if (node->isDevice())
            return int(node->device->GetPushOnConnect())*2;
        break;
        default:
          break;
        }
    }

  if (role==Qt::DisplayRole)
    {
      switch (index.column())
        {
        case qIGTLIODevicesModel::NameColumn:
          {
            return QString::fromStdString(node->GetName());
            break;
          }
        case qIGTLIODevicesModel::StatusColumn:
          {
            if (node->isDevice())
              return QString::fromStdString(node->device->GetDeviceType());
            break;
          }
        default:
          break;
        }
    }
  return QVariant();
}

//-----------------------------------------------------------------------------
bool qIGTLIODevicesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role!=Qt::CheckStateRole)
    return false;

  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(index);

  if (!node)
    {
      return false;
    }

  if (role==Qt::CheckStateRole)
    {
      switch (index.column())
        {
        case qIGTLIODevicesModel::PushOnConnectColumn:
          if (node->isDevice())
            {
            node->device->SetPushOnConnect(value.toBool());
            return true;
            }
        break;
        default:
          break;
        }
    }

  return false;
}

//-----------------------------------------------------------------------------
Qt::ItemFlags qIGTLIODevicesModel::flags(const QModelIndex &index) const
{
  switch (index.column())
    {
    case qIGTLIODevicesModel::PushOnConnectColumn:
      {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
      break;
      }
    default:
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
QVariant qIGTLIODevicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
      return HeaderLabels[section];
    }

  return QVariant();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIODevicesModel::index(int row, int column, const QModelIndex &parent) const
{
  qIGTLIODevicesModelNode* parentNode = this->getNodeFromIndex(parent);
  qIGTLIODevicesModelNode* node = NULL;

  if (!parentNode) // connector
    {
      return QModelIndex();
    }

  node = parentNode->GetChild(row);

  if (node)
    {
    return this->createIndex(row, column, node);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIODevicesModel::parent(const QModelIndex &index) const
{
  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(index);
  if (!node)
    {
      return QModelIndex();
    }

  qIGTLIODevicesModelNode* parentNode = node->GetParent();
  if (parentNode && !parentNode->isRoot())
    {
      return this->createIndex(parentNode->GetSiblingIndex(), 0, parentNode);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::resetModel()
{
  this->beginResetModel();
  RootNode = qIGTLIODevicesModelNode::createRoot(Logic);
  this->endResetModel();
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::setLogic(igtlio::vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << igtlio::vtkIGTLIOLogic::ConnectionAddedEvent
          << igtlio::vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionEvent(vtkObject*, unsigned long, void*, void*)));
    }

  this->Logic = logic;

  this->resetModel();
}

void qIGTLIODevicesModel::setSelectionModel(QItemSelectionModel *selectionModel)
{
  SelectionModel = selectionModel;
}

QItemSelectionModel *qIGTLIODevicesModel::selectionModel()
{
  return SelectionModel;
}

//qIGTLIODevicesModelNode *qIGTLIODevicesModel::selectedNode()
//{
//  return seleSelectionModel->currentIndex();

//}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::ReconnectConnector(igtlio::vtkIGTLIOConnector* oldConnector, igtlio::vtkIGTLIOConnector* newConnector)
{
  foreach(int evendId, QList<int>()
          << igtlio::vtkIGTLIOConnector::ConnectedEvent
          << igtlio::vtkIGTLIOConnector::DisconnectedEvent
          << igtlio::vtkIGTLIOConnector::ActivatedEvent
          << igtlio::vtkIGTLIOConnector::DeactivatedEvent
          << igtlio::vtkIGTLIOConnector::NewDeviceEvent
          << igtlio::vtkIGTLIOConnector::DeviceModifiedEvent
          << igtlio::vtkIGTLIOConnector::RemovedDeviceEvent
          )
    {
    qvtkReconnect(oldConnector, newConnector, evendId,
                  this, SLOT(onConnectorEvent(vtkObject*, unsigned long, void*, void*)));
    }
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::onConnectionEvent(vtkObject* caller, unsigned long event , void*, void* connector)
{
  if (event==igtlio::vtkIGTLIOLogic::ConnectionAddedEvent)
    {
      igtlio::vtkIGTLIOConnector* c = static_cast<igtlio::vtkIGTLIOConnector*>(connector);
      this->ReconnectConnector(NULL, c);
      this->resetModel();
    }
  if (event==igtlio::vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
      igtlio::vtkIGTLIOConnector* c = static_cast<igtlio::vtkIGTLIOConnector*>(connector);
      this->ReconnectConnector(c, NULL);
      this->resetModel();
    }
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::onConnectorEvent(vtkObject* caller, unsigned long event , void*, void* c)
{
  if (event==igtlio::vtkIGTLIOConnector::NewDeviceEvent)
    {
      igtlio::Device* device = static_cast<igtlio::Device*>(c);

      qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
      QModelIndex parent = this->createIndex(node->GetParent()->GetSiblingIndex(), 0, node->GetParent());

      this->beginInsertRows(parent, node->GetSiblingIndex(), node->GetSiblingIndex());
      this->endInsertRows();

      QModelIndex index = this->index(node->GetSiblingIndex(), 0, parent);
      SelectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    }
  if (event==igtlio::vtkIGTLIOConnector::RemovedDeviceEvent)
    {
     igtlio::Device* device = static_cast<igtlio::Device*>(c);

     qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
     QModelIndex parent = this->createIndex(node->GetParent()->GetSiblingIndex(), 0, node->GetParent());

     this->beginRemoveRows(parent, node->GetSiblingIndex(), node->GetSiblingIndex());
     this->endRemoveRows();
    }
  if (event==igtlio::vtkIGTLIOConnector::DeviceModifiedEvent)
    {
      // TODO: this event is never emittd, and never will. The vtkCommand::ModifiedEvent is emitted
      // from each device, and each of the must be listened to.
     igtlio::Device* device = static_cast<igtlio::Device*>(c);
     qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
     QModelIndex bindex = this->createIndex(node->GetSiblingIndex(), 0, node);
     QModelIndex eindex = this->createIndex(node->GetSiblingIndex(), this->columnCount(bindex), node);

     emit dataChanged(bindex, eindex);
    }

}

//-----------------------------------------------------------------------------
qIGTLIODevicesModelNode* qIGTLIODevicesModel::getNodeFromIndex(const QModelIndex &index) const
{
  if (!index.isValid())
    return RootNode.data();
  return static_cast<qIGTLIODevicesModelNode*>(index.internalPointer());
}
