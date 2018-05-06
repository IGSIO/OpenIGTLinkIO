#ifndef VTKIGTLIONODE_H
#define VTKIGTLIONODE_H

// igtlio includes
#include "igtlioGUIExport.h"

#include "igtlioLogic.h"
#include "igtlioConnector.h"

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

  static qIGTLIODevicesModelNodePointer createRoot(igtlioLogic* logic_);
  bool operator==(const qIGTLIODevicesModelNode& rhs) const;
  std::string GetName();
  qIGTLIODevicesModelNode* GetChild(int row);
  qIGTLIODevicesModelNode* GetParent();

  int GetNumberOfChildren() const;
  /// get the index of this node among its siblings
  int GetSiblingIndex() const;
  void PrintSelf(ostream& os, vtkIndent indent);

  qIGTLIODevicesModelNode* FindDeviceNode(igtlioDevice* device_);

  igtlioDevice* device;
  igtlioConnector* connector;

private:
  qIGTLIODevicesModelNode* GetNode(igtlioConnector* connector=NULL, igtlioDevice::MESSAGE_DIRECTION group=igtlioDevice::NUM_MESSAGE_DIRECTION, igtlioDevice* device=NULL);
  qIGTLIODevicesModelNode(qIGTLIODevicesModelNode* parent_, igtlioLogic* logic_, igtlioConnector* connector_=NULL, igtlioDevice::MESSAGE_DIRECTION group_=igtlioDevice::NUM_MESSAGE_DIRECTION, igtlioDevice* device_=NULL);
  std::vector<igtlioDevicePointer> GetDevicesInGroup() const;
  NODE_TYPE type;
  igtlioLogic* logic;
  igtlioDevice::MESSAGE_DIRECTION group;
  mutable std::set<qIGTLIODevicesModelNodePointer> Children;
  qIGTLIODevicesModelNode* Parent;
};



#endif // VTKIGTLIONODE_H
