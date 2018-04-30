#include "igtlioUtilities.h"
#include "igtlioDevice.h"

std::string igtlioDeviceKeyType::GetBaseTypeName() const
{
  int pos = (int)type.find("_");
  if (pos>=0)
    return type.substr(pos+1);
  return type;
}

igtlioDeviceKeyType igtlioDeviceKeyType::CreateDeviceKey(igtl::MessageBase::Pointer message)
{
  if (!message)
    return igtlioDeviceKeyType();
  return igtlioDeviceKeyType(message->GetDeviceType(), message->GetDeviceName());
}

igtlioDeviceKeyType igtlioDeviceKeyType::CreateDeviceKey(igtlioDevicePointer device)
{
  if (!device)
    return igtlioDeviceKeyType();
  return igtlioDeviceKeyType(device->GetDeviceType(), device->GetDeviceName());
}


bool operator<(const igtlioDeviceKeyType &lhs, const igtlioDeviceKeyType &rhs)
{
  if (lhs.GetBaseTypeName() > rhs.GetBaseTypeName())
      return false;
  if (lhs.GetBaseTypeName() < rhs.GetBaseTypeName())
      return true;
  return (lhs.name < rhs.name);
}

bool operator==(const igtlioDeviceKeyType &lhs, const igtlioDeviceKeyType &rhs)
{
  return (lhs.GetBaseTypeName()==rhs.GetBaseTypeName()) &&
      (lhs.name==rhs.name);
}
