#include "igtlioBaseConverter.h"


namespace igtlio
{

//---------------------------------------------------------------------------
int BaseConverter::IGTLtoHeader(igtl::MessageBase::Pointer source, BaseConverter::HeaderData *header)
{
  header->deviceName = source->GetDeviceName();
  // get timestamp
  if (IGTLToTimestamp(source, header) == 0)
    return 0;
  return 1;
}

int BaseConverter::HeadertoIGTL(const BaseConverter::HeaderData &header, igtl::MessageBase::Pointer *dest)
{
  (*dest)->SetDeviceName(header.deviceName.c_str());
  return 1;
}

  int BaseConverter::IGTLToTimestamp(igtl::MessageBase::Pointer msg, HeaderData *dest)
{
  // Save OpenIGTLink time stamp
  igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
  msg->GetTimeStamp(ts);
  dest->timestamp = ts->GetTimeStamp();
  return 1;
}

} // namespace igtlio
