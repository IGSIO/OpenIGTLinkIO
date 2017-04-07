#include "igtlioBaseConverter.h"


namespace igtlio
{

static std::string equipment_uid_name = "EQUIPMENT_UID";
static std::string equipment_type_name = "EQUIPMENT_TYPE";

//---------------------------------------------------------------------------
int BaseConverter::IGTLtoHeader(igtl::MessageBase::Pointer source, BaseConverter::HeaderData *header)
{
  header->deviceName = source->GetDeviceName();
  // get timestamp
  if (IGTLToTimestamp(source, header) == 0)
    return 0;

  source->GetMetaDataElement(equipment_uid_name, header->equipmentId);
  std::string equipmentTypeString;
  source->GetMetaDataElement(equipment_type_name, equipmentTypeString);

  if(header->equipmentId.empty())
  {
	header->equipmentType = BaseConverter::UNKNOWN;
  }
  if(equipmentTypeString.empty())
  {
	header->equipmentId = "unknown";
  } else {
	  //TODO is this right????
	  std::string::size_type sz;   // alias of size_t
	  header->equipmentType = static_cast<EQUIPMENT_TYPE>(std::stoi(equipmentTypeString, &sz));
  }

  return 1;
}

int BaseConverter::HeadertoIGTL(const BaseConverter::HeaderData &header, igtl::MessageBase::Pointer *dest)
{
  (*dest)->SetDeviceName(header.deviceName.c_str());
  (*dest)->SetMetaDataElement(equipment_uid_name, IANA_TYPE_US_ASCII, header.equipmentId);
  (*dest)->SetMetaDataElement(equipment_type_name, header.equipmentType);
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
