#include "igtlioUtilities.h"
#include "igtlioDevice.h"

namespace igtlio
{

std::string DeviceKeyType::GetBaseTypeName() const
{
  int pos = type.find("_");
  if (pos>=0)
    return type.substr(pos+1);
  return type;
}

DeviceKeyType CreateDeviceKey(igtl::MessageBase::Pointer message)
{
  if (!message)
    return DeviceKeyType();
  return DeviceKeyType(message->GetDeviceType(), message->GetDeviceName());
}

DeviceKeyType CreateDeviceKey(DevicePointer device)
{
  if (!device)
    return DeviceKeyType();
  return DeviceKeyType(device->GetDeviceType(), device->GetDeviceName());
}


bool operator<(const DeviceKeyType &lhs, const DeviceKeyType &rhs)
{
  if (lhs.GetBaseTypeName() > rhs.GetBaseTypeName())
      return false;
  if (lhs.GetBaseTypeName() < rhs.GetBaseTypeName())
      return true;
  return (lhs.name < rhs.name);
}

bool operator==(const DeviceKeyType &lhs, const DeviceKeyType &rhs)
{
  return (lhs.GetBaseTypeName()==rhs.GetBaseTypeName()) &&
      (lhs.name==rhs.name);
}




} // namespace igtlio
