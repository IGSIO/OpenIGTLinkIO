#ifndef IGTLIOUTILITIES_H
#define IGTLIOUTILITIES_H

#include "igtlioLogicExport.h"
#include <string>
#include "igtlMessageBase.h"
#include "vtkSmartPointer.h"

namespace igtlio
{
typedef vtkSmartPointer<class Device> DevicePointer;

enum OPENIGTLINKIO_LOGIC_EXPORT SYNCHRONIZATION_TYPE
{
  BLOCKING,
  ASYNCHRONOUS
};

///
/// Uniquely identify a Device by both its name and type.
/// This enables broadcast Devices (with empty name) of different types
/// to be stored in the same structures.
///
struct OPENIGTLINKIO_LOGIC_EXPORT DeviceKeyType
{
  explicit DeviceKeyType() {}
  explicit DeviceKeyType(const std::string& type_, const std::string& name_) :
    type(type_), name(name_) {}
  std::string type;
  std::string name;

public:
  std::string GetBaseTypeName() const;
};
//typedef std::pair<std::string, std::string> DeviceKeyType;
OPENIGTLINKIO_LOGIC_EXPORT DeviceKeyType CreateDeviceKey(igtl::MessageBase::Pointer message);
OPENIGTLINKIO_LOGIC_EXPORT DeviceKeyType CreateDeviceKey(DevicePointer device);
bool operator==(const DeviceKeyType& lhs, const DeviceKeyType& rhs);
bool operator<(const DeviceKeyType& lhs, const DeviceKeyType& rhs);

} // namespace igtlio



#endif // IGTLIOUTILITIES_H
