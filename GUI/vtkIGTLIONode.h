#ifndef VTKIGTLIONODE_H
#define VTKIGTLIONODE_H

// igtlio includes
#include "igtlioGUIExport.h"

#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"

#include <QSharedPointer>

typedef QSharedPointer<class qIGTLIODevicesModelNode> qIGTLIODevicesModelNodePointer;

//class vtkIGTLIONode
//{
//};

/// Convenience class for holding each tree item.
///
/// There are three levels/types of nodes: connector, group and device.
///  - connector: only connector field defined
///  - group: connector and group fields defined
///  - device: connect, group, device fields defined
class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicesModelNode
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

  static qIGTLIODevicesModelNodePointer createRoot(igtlio::vtkIGTLIOLogic* logic_);
  bool operator==(const qIGTLIODevicesModelNode& rhs) const;
  std::string GetName();
  qIGTLIODevicesModelNode* GetChild(int row);
  qIGTLIODevicesModelNode* GetParent();

  int GetNumberOfChildren() const;
  /// get the index of this node among its siblings
  int GetSiblingIndex() const;
  void PrintSelf(ostream& os, vtkIndent indent);

  qIGTLIODevicesModelNode* FindDeviceNode(igtlio::Device* device_);

  igtlio::Device* device;
  igtlio::vtkIGTLIOConnector* connector;

private:
  qIGTLIODevicesModelNode* GetNode(igtlio::vtkIGTLIOConnector* connector=NULL, igtlio::Device::MESSAGE_DIRECTION group=igtlio::Device::NUM_MESSAGE_DIRECTION, igtlio::Device* device=NULL);
  qIGTLIODevicesModelNode(qIGTLIODevicesModelNode* parent_, igtlio::vtkIGTLIOLogic* logic_, igtlio::vtkIGTLIOConnector* connector_=NULL, igtlio::Device::MESSAGE_DIRECTION group_=igtlio::Device::NUM_MESSAGE_DIRECTION, igtlio::Device* device_=NULL);
  std::vector<igtlio::DevicePointer> GetDevicesInGroup() const;
  NODE_TYPE type;
  igtlio::vtkIGTLIOLogic* logic;
  igtlio::Device::MESSAGE_DIRECTION group;
  mutable std::set<qIGTLIODevicesModelNodePointer> Children;
  qIGTLIODevicesModelNode* Parent;
};



#endif // VTKIGTLIONODE_H
