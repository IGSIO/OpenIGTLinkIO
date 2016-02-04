#include "qIGTLIODevicesModel.h"

// Qt includes
#include <QDebug>
#include <QMap>
#include <QMimeData>
#include <QSharedPointer>
#include <QStack>
#include <QStringList>
#include <QVector>

// OpenIGTLinkIF GUI includes
#include <qIGTLIODevicesModel.h>

#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"

// VTK includes
#include <vtkVariantArray.h>
#include "qIGTLIODevicesModel.h"
#include <vtkEventQtSlotConnect.h>

/// Convenience class for holding each tree item.
///
/// There are three levels/types of nodes: connector, group and device.
///  - connector: only connector field defined
///  - group: connector and group fields defined
///  - device: connect, group, device fields defined
class qIGTLIODevicesModelNode
{
public:
  enum NODE_TYPE {
    NODE_TYPE_ROOT,
    NODE_TYPE_CONNECTOR,
    NODE_TYPE_GROUP,
    NODE_TYPE_DEVICE,
  };
  bool isRoot() const { return type==NODE_TYPE_ROOT; }
  bool isConnector() const { return type==NODE_TYPE_CONNECTOR; }
  bool isGroup() const { return type==NODE_TYPE_GROUP; }
  bool isDevice() const { return type==NODE_TYPE_DEVICE; }

  qIGTLIODevicesModelNode(vtkIGTLIOLogic* logic_, vtkIGTLIOConnector* connector_=NULL, vtkIGTLIODevice::MESSAGE_DIRECTION group_=vtkIGTLIODevice::NUM_MESSAGE_DIRECTION, vtkIGTLIODevice* device_=NULL)
  {
    logic = logic_;
    connector = connector_;
    group = group_;
    device = device_;

    if (device!=NULL)
      {
        type = NODE_TYPE_DEVICE;
      }
    else if (group!=vtkIGTLIODevice::NUM_MESSAGE_DIRECTION)
      {
        type = NODE_TYPE_GROUP;
      }
    else if (connector!=NULL)
      {
        type = NODE_TYPE_CONNECTOR;
      }
    else
      {
        type = NODE_TYPE_ROOT;
      }
  }

  bool operator==(const qIGTLIODevicesModelNode& rhs) const
  {
    return type==rhs.type &&
        logic==rhs.logic &&
        connector==rhs.connector &&
        group==rhs.group &&
        device==rhs.device;
  }

  std::string GetName()
  {
    if (this->isRoot())
      {
      return "root";
      }
    if (this->isConnector())
      {
      return connector->GetName();
      }
    if (this->isGroup())
      {
        if (group==vtkIGTLIODevice::MESSAGE_DIRECTION_IN)
          return "IN";
        if (group==vtkIGTLIODevice::MESSAGE_DIRECTION_OUT)
          return "OUT";
        return "???";
      }
    if (this->isDevice())
      {
      return device->GetDeviceName();
      }
    // device
    return 0;
  }

  std::vector<vtkIGTLIODevicePointer> GetDevicesInGroup() const
  {
    std::vector<vtkIGTLIODevicePointer> retval;
    for (int i=0; i<connector->GetNumberOfDevices(); ++i)
      {
        vtkIGTLIODevicePointer d = connector->GetDevice(i);
        if (d->GetMessageDirection()==group)
          retval.push_back(d);
      }
    return retval;
  }

  int GetNumberOfChildren() const
  {
    if (this->isRoot())
      {
      return logic->GetNumberOfConnectors();
      }
    if (this->isConnector())
      {
      return vtkIGTLIODevice::NUM_MESSAGE_DIRECTION;
      }
    if (this->isGroup())
      {
      return this->GetDevicesInGroup().size();
      }
    // device
    return 0;
  }

  /// get the index of this node among its siblings
  int GetSiblingIndex() const
  {
    if (this->isRoot())
      {
      return -1;
      }
    if (this->isConnector())
      {
        for (int i=0; i<logic->GetNumberOfConnectors(); ++i)
          {
            if (logic->GetConnector(i) == connector)
              return i;
          }
      }
    if (this->isGroup())
      {
        return group;
      }
    if (this->isDevice())
      {
        std::vector<vtkIGTLIODevicePointer> devices = this->GetDevicesInGroup();
        for (int i=0; i<devices.size(); ++i)
          {
            if (devices[i] == device)
              return i;
          }

      }
    // root or error
    return -1;
  }

  void PrintSelf(ostream& os, vtkIndent indent)
  {
    os << indent << type << "\n"
       << indent << "Connector: " << ((connector) ? connector->GetName() : "-" ) << "\n"
       << indent << "Group: " << group << "\n"
       << indent << "Device: " << ((device) ? device->GetDeviceName() : "-" ) << "\n";
  }


  NODE_TYPE type;
  vtkIGTLIOLogic* logic;
  vtkIGTLIOConnector* connector;
  vtkIGTLIODevice::MESSAGE_DIRECTION group;
  vtkIGTLIODevice* device;
};


//------------------------------------------------------------------------------
qIGTLIODevicesModel::qIGTLIODevicesModel(QObject *vparent)
  :QAbstractItemModel(vparent)
{
  this->Connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();
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
  std::cout << msg.toStdString() << std::endl;
}

//-----------------------------------------------------------------------------
int qIGTLIODevicesModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() > 0)
    return 0;

//  return 1;
  dmsg("rowcount B");
  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(parent);
  dmsg("rowcount E");
  int r = node->GetNumberOfChildren();
  std::cout << "children " << r << std::endl;
  dmsg("rowcount E2");
  return r;
}

//------------------------------------------------------------------------------
QVariant qIGTLIODevicesModel::data(const QModelIndex &index, int role) const
{
  if (role!=Qt::DisplayRole)
    return QVariant();

//  return QVariant();
  dmsg("data b");
  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(index);
  dmsg("data e1");

  if (!node)
    {
    return QVariant();
    }
  switch (index.column())
    {
    case qIGTLIODevicesModel::NameColumn:
      {
        return QString::fromStdString(node->GetName());
//      return QString("%1").arg(node->type);
//      return QString::fromStdString(device->GetDeviceName());
      break;
      }
//    case qIGTLIODevicesModel::TypeColumn:
//      {
//      Q_ASSERT(cnode->GetType() < vtkIGTLIOConnector::NUM_TYPE);
//      return QString::fromStdString(vtkIGTLIOConnector::ConnectorTypeStr[cnode->GetType()]);
//      break;
//      }
//    case qIGTLIODevicesModel::StatusColumn:
//      {
//      Q_ASSERT(cnode->GetState() < vtkIGTLIOConnector::NUM_STATE);
//      return QString::fromStdString(vtkIGTLIOConnector::ConnectorStateStr[cnode->GetState()]);
//      break;
//      }
//    case qIGTLIODevicesModel::HostnameColumn:
//      {
//      if (cnode->GetType() == vtkIGTLIOConnector::TYPE_CLIENT)
//        {
//        return QString::fromStdString(cnode->GetServerHostname());
//        }
//      else
//        {
//        return QString("--");
//        }
//      break;
//      }
//    case qIGTLIODevicesModel::PortColumn:
//      {
//      return QString("%1").arg(cnode->GetServerPort());
//      break;
//      }
    default:
      break;
    }
  return QVariant();
}


//-----------------------------------------------------------------------------
Qt::ItemFlags qIGTLIODevicesModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
  dmsg("index b");
  qIGTLIODevicesModelNode* parentNode = this->getNodeFromIndex(parent);
  qIGTLIODevicesModelNode* node = NULL;
  dmsg("index i1 ");
  std::cout << "pn " << parentNode << std::endl;
  std::cout << "row " << row << std::endl;

  if (!parentNode) // connector
    {
      return QModelIndex();
    }

  dmsg("parent::");
  parentNode->PrintSelf(std::cout, vtkIndent(1));
  std::cout << std::endl;

  if (parentNode->isRoot())
    {
      node = this->GetNode(Logic->GetConnector(row));
    }
  if (parentNode->isConnector())
    {
      node = this->GetNode(parentNode->connector, static_cast<vtkIGTLIODevice::MESSAGE_DIRECTION>(row));
    }
  if (parentNode->isGroup())
    {
      node = this->GetNode(parentNode->connector, parentNode->group, parentNode->GetDevicesInGroup()[row]);
    }

  std::cout << "node:: " << node << std::endl;
  dmsg("index e1");
  if (node)
    {
      node->PrintSelf(std::cout, vtkIndent(11));
      std::cout << std::endl;
    return this->createIndex(row, column, node);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIODevicesModel::parent(const QModelIndex &index) const
{
  dmsg("parent b");
  qIGTLIODevicesModelNode* node = this->getNodeFromIndex(index);
  qIGTLIODevicesModelNode* parentNode = NULL;

  if (!node)
    {
      return QModelIndex();
    }

  if (node->isConnector())
    {
      return QModelIndex();
    }

  if (node->isGroup())
    {
      parentNode = this->GetNode(node->connector);
    }
  if (node->isDevice())
    {
      parentNode = this->GetNode(node->connector, node->group);
    }
  dmsg("parent e1");

  if (parentNode)
    {
    return this->createIndex(parentNode->GetSiblingIndex(), 0, parentNode);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::resetModel()
{
  dmsg("resetModel b");
  this->beginResetModel();
  this->endResetModel();
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::setLogic(vtkIGTLIOLogicPointer logic)
{
  dmsg("setLogic b");

  this->Logic = logic;

  this->Connections->Connect(Logic,
                             vtkIGTLIOLogic::ConnectionAddedEvent,
                             this,
                             SLOT(onConnectionEvent(vtkObject*, unsigned long, void*, void*)));
  this->Connections->Connect(Logic,
                             vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent,
                             this,
                             SLOT(onConnectionEvent(vtkObject*, unsigned long, void*, void*)));

  Nodes.clear();
  qIGTLIODevicesModelNodePointer rootNode;
  rootNode.reset(new qIGTLIODevicesModelNode(Logic));
  Nodes.insert(rootNode);
  RootNode = rootNode;
  dmsg("setLogic e");
}

//-----------------------------------------------------------------------------
void qIGTLIODevicesModel::onConnectionEvent(vtkObject* caller, unsigned long event , void*, void*)
{
  if (event==vtkIGTLIOLogic::ConnectionAddedEvent)
    {
      std::cout << "qIGTLIODevicesModel on add connected event" << std::endl;
      this->resetModel();
    }
  if (event==vtkIGTLIOLogic::ConnectionAboutToBeRemovedEvent)
    {
      std::cout << "qIGTLIODevicesModel on remove connected event" << std::endl;
      this->resetModel();
    }
}

//-----------------------------------------------------------------------------
qIGTLIODevicesModelNode* qIGTLIODevicesModel::getNodeFromIndex(const QModelIndex &index) const
{
//  dmsg("getNodeFromIndex b");
  if (!index.isValid())
    return RootNode.data();
  return static_cast<qIGTLIODevicesModelNode*>(index.internalPointer());
}

qIGTLIODevicesModelNode *qIGTLIODevicesModel::GetNode(vtkIGTLIOConnector *connector, vtkIGTLIODevice::MESSAGE_DIRECTION group, vtkIGTLIODevice *device) const
{
//  dmsg("GetNode b");
  qIGTLIODevicesModelNodePointer node;
  node.reset(new qIGTLIODevicesModelNode(Logic, connector, group, device));

  for (std::set<qIGTLIODevicesModelNodePointer>::iterator i=Nodes.begin(); i!=Nodes.end(); ++i)
    {
      if (*i->data() == *node)
        return i->data();
    }

  Nodes.insert(node);
  return node.data();
}


