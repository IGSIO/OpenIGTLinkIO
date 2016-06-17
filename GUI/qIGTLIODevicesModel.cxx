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

#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
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
//  std::cout << msg.toStdString() << std::endl;
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
void qIGTLIODevicesModel::setLogic(vtkIGTLIOLogicPointer logic)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOLogic::ConnectionAddedEvent
          << vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
    qvtkReconnect(this->Logic, logic, evendId,
                  this, SLOT(onConnectionEvent(vtkObject*, void*, unsigned long, void*)));
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
void qIGTLIODevicesModel::ReconnectConnector(vtkIGTLIOConnector* oldConnector, vtkIGTLIOConnector* newConnector)
{
  foreach(int evendId, QList<int>()
          << vtkIGTLIOConnector::ConnectedEvent
          << vtkIGTLIOConnector::DisconnectedEvent
          << vtkIGTLIOConnector::ActivatedEvent
          << vtkIGTLIOConnector::DeactivatedEvent
          << vtkIGTLIOConnector::NewDeviceEvent
          << vtkIGTLIOConnector::DeviceModifiedEvent
          << vtkIGTLIOConnector::RemovedDeviceEvent
          )
    {
    qvtkReconnect(oldConnector, newConnector, evendId,
                  this, SLOT(onConnectorEvent(vtkObject*, void*, unsigned long, void*)));
    }
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::onConnectionEvent(vtkObject* caller, void* connector, unsigned long event , void*)
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
void qIGTLIODevicesModel::onConnectorEvent(vtkObject* caller, void* c, unsigned long event , void*)
{
  if (event==vtkIGTLIOConnector::NewDeviceEvent)
    {
      vtkIGTLIODevice* device = static_cast<vtkIGTLIODevice*>(c);

      qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
      std::cout << "added: " << node->GetName() << std::endl;
      QModelIndex parent = this->createIndex(node->GetParent()->GetSiblingIndex(), 0, node->GetParent());

      this->beginInsertRows(parent, node->GetSiblingIndex(), node->GetSiblingIndex());
      this->endInsertRows();

      QModelIndex index = this->index(node->GetSiblingIndex(), 0, parent);
      SelectionModel->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    }
  if (event==vtkIGTLIOConnector::RemovedDeviceEvent)
    {
     vtkIGTLIODevice* device = static_cast<vtkIGTLIODevice*>(c);

     qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
     std::cout << "removed: " << node->GetName() << std::endl;
     QModelIndex parent = this->createIndex(node->GetParent()->GetSiblingIndex(), 0, node->GetParent());

     this->beginRemoveRows(parent, node->GetSiblingIndex(), node->GetSiblingIndex());
     this->endRemoveRows();
    }
  if (event==vtkIGTLIOConnector::DeviceModifiedEvent)
    {
     vtkIGTLIODevice* device = static_cast<vtkIGTLIODevice*>(c);
     qIGTLIODevicesModelNode* node = RootNode->FindDeviceNode(device);
     std::cout << "modified: " << node->GetName() << std::endl;
     QModelIndex bindex = this->createIndex(node->GetSiblingIndex(), 0, node);
     QModelIndex eindex = this->createIndex(node->GetSiblingIndex(), this->columnCount(bindex), node);

     emit dataChanged(bindex, eindex);
    }
//  else
//    {
//      this->resetModel();
//      emit dataChanged(QModelIndex(), QModelIndex());
//    }
}

//-----------------------------------------------------------------------------
qIGTLIODevicesModelNode* qIGTLIODevicesModel::getNodeFromIndex(const QModelIndex &index) const
{
  if (!index.isValid())
    return RootNode.data();
  return static_cast<qIGTLIODevicesModelNode*>(index.internalPointer());
}
