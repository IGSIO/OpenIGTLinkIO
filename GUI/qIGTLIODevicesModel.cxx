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
#include "vtkIGTLIONode.h"

// VTK includes
#include <vtkVariantArray.h>
#include "qIGTLIODevicesModel.h"
#include <vtkEventQtSlotConnect.h>



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
//  dmsg("rowcount E");
  int r = node->GetNumberOfChildren();
//  std::cout << "children " << r << std::endl;
//  dmsg("rowcount E2");
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
//  dmsg("index i1 ");
//  std::cout << "pn " << parentNode << std::endl;
//  std::cout << "row " << row << std::endl;

  if (!parentNode) // connector
    {
      return QModelIndex();
    }

//  dmsg("parent::");
//  parentNode->PrintSelf(std::cout, vtkIndent(1));
//  std::cout << std::endl;

  node = parentNode->GetChild(row);

//  std::cout << "node:: " << node << std::endl;
//  dmsg("index e1");
  if (node)
    {
//      node->PrintSelf(std::cout, vtkIndent(11));
//      std::cout << std::endl;
    return this->createIndex(row, column, node);
    }

  return QModelIndex();
}

//-----------------------------------------------------------------------------
QModelIndex qIGTLIODevicesModel::parent(const QModelIndex &index) const
{
  dmsg("parent b");
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

  RootNode = qIGTLIODevicesModelNode::createRoot(Logic);
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
