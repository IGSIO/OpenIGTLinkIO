#include "vtkIGTLIONode.h"
#include "qIGTLIOGuiUtilities.h"

qIGTLIODevicesModelNodePointer qIGTLIODevicesModelNode::createRoot(igtlio::vtkIGTLIOLogic *logic_)
{
  qIGTLIODevicesModelNodePointer retval(new qIGTLIODevicesModelNode(NULL, logic_));
  return retval;
}

qIGTLIODevicesModelNode::qIGTLIODevicesModelNode(qIGTLIODevicesModelNode *parent_, igtlio::vtkIGTLIOLogic *logic_, igtlio::vtkIGTLIOConnector *connector_, igtlio::vtkIGTLIODevice::MESSAGE_DIRECTION group_, igtlio::vtkIGTLIODevice *device_)
{
  Parent = parent_;
  logic = logic_;
  connector = connector_;
  group = group_;
  device = device_;

  if (device!=NULL)
    {
      type = NODE_TYPE_DEVICE;
    }
  else if (group!=igtlio::vtkIGTLIODevice::NUM_MESSAGE_DIRECTION)
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

bool qIGTLIODevicesModelNode::operator==(const qIGTLIODevicesModelNode &rhs) const
{
  return type==rhs.type &&
      logic==rhs.logic &&
      connector==rhs.connector &&
      group==rhs.group &&
      device==rhs.device;
}

std::string qIGTLIODevicesModelNode::GetName()
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
      if (group==igtlio::vtkIGTLIODevice::MESSAGE_DIRECTION_IN)
        return "IN";
      if (group==igtlio::vtkIGTLIODevice::MESSAGE_DIRECTION_OUT)
        return "OUT";
      return "???";
    }
  if (this->isDevice())
    {
      return convertDeviceNameToDisplay(device->GetDeviceName());
    }
  // device
  return 0;
}

qIGTLIODevicesModelNode *qIGTLIODevicesModelNode::GetChild(int row)
{
  qIGTLIODevicesModelNode* node;
  if (this->isRoot())
    {
      node = this->GetNode(logic->GetConnector(row));
    }
  if (this->isConnector())
    {
      node = this->GetNode(connector, static_cast<igtlio::vtkIGTLIODevice::MESSAGE_DIRECTION>(row));
    }
  if (this->isGroup())
    {
      node = this->GetNode(connector, group, this->GetDevicesInGroup()[row]);
    }
  return node;
}

qIGTLIODevicesModelNode *qIGTLIODevicesModelNode::GetParent()
{
  return Parent;
}

qIGTLIODevicesModelNode *qIGTLIODevicesModelNode::GetNode(igtlio::vtkIGTLIOConnector *connector, igtlio::vtkIGTLIODevice::MESSAGE_DIRECTION group, igtlio::vtkIGTLIODevice *device)
{
  //  dmsg("GetNode b");
  qIGTLIODevicesModelNodePointer node(new qIGTLIODevicesModelNode(this, logic, connector, group, device));

  for (std::set<qIGTLIODevicesModelNodePointer>::iterator i=Children.begin(); i!=Children.end(); ++i)
    {
      if (*i->data() == *node)
        return i->data();
    }

  Children.insert(node);
  return node.data();
}

std::vector<igtlio::vtkIGTLIODevicePointer> qIGTLIODevicesModelNode::GetDevicesInGroup() const
{
  std::vector<igtlio::vtkIGTLIODevicePointer> retval;
  for (unsigned int i=0; i<connector->GetNumberOfDevices(); ++i)
    {
      igtlio::vtkIGTLIODevicePointer d = connector->GetDevice(i);
      if (d->GetMessageDirection()==group)
        retval.push_back(d);
    }
  return retval;
}

int qIGTLIODevicesModelNode::GetNumberOfChildren() const
{
  if (this->isRoot())
    {
      return logic->GetNumberOfConnectors();
    }
  if (this->isConnector())
    {
      return igtlio::vtkIGTLIODevice::NUM_MESSAGE_DIRECTION;
    }
  if (this->isGroup())
    {
      return this->GetDevicesInGroup().size();
    }
  // device
  return 0;
}

int qIGTLIODevicesModelNode::GetSiblingIndex() const
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
      std::vector<igtlio::vtkIGTLIODevicePointer> devices = this->GetDevicesInGroup();
      for (unsigned int i=0; i<devices.size(); ++i)
        {
          if (devices[i] == device)
            return i;
        }

    }
  // root or error
  return -1;
}

void qIGTLIODevicesModelNode::PrintSelf(std::ostream &os, vtkIndent indent)
{
  os << indent << type << "\n"
     << indent << "Connector: " << ((connector) ? connector->GetName() : "-" ) << "\n"
     << indent << "Group: " << group << "\n"
     << indent << "Device: " << ((device) ? device->GetDeviceName() : "-" ) << "\n";
}

qIGTLIODevicesModelNode *qIGTLIODevicesModelNode::FindDeviceNode(igtlio::vtkIGTLIODevice *device_)
{
  if (this->isDevice() && device==device_)
    return this;

  for (int i=0; i<this->GetNumberOfChildren(); ++i)
    {
      qIGTLIODevicesModelNode* hit = this->GetChild(i)->FindDeviceNode(device_);
      if (hit)
        return hit;
    }
  return NULL;
}
