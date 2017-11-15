#include "igtlioBaseConverter.h"


namespace igtlio
{

static std::string equipment_uid_name = "EQUIPMENT_UID";
static std::string equipment_type_name = "EQUIPMENT_TYPE";
static std::string stream_id_name = "STREAM_ID";

//---------------------------------------------------------------------------
int BaseConverter::IGTLtoHeader(igtl::MessageBase::Pointer source, BaseConverter::HeaderData *header)
{
  header->deviceName = source->GetDeviceName();
  // get timestamp
  if (IGTLToTimestamp(source, header) == 0)
    return 0;

  source->GetMetaDataElement(equipment_uid_name, header->equipmentId);
  source->GetMetaDataElement(stream_id_name, header->streamId);
  std::string equipmentTypeString;
  source->GetMetaDataElement(equipment_type_name, equipmentTypeString);

  // Added stream_id. OpenIGTLink DEVICE_NAME can possibly be used, but a separate tag is probably best.
  // stream_id can then be used to connect components with calibration messages (sent as transform messages).
  // stream_id for to and from should be placed the header of transform messages

  if(header->equipmentId.empty())
  {
	  header->equipmentId = "unknown";
  }
  if(header->streamId.empty())
  {
	  header->streamId = "unknown";
  }
  if(equipmentTypeString.empty())
  {
	header->equipmentType = BaseConverter::UNKNOWN;
  } else {
	  header->equipmentType = static_cast<EQUIPMENT_TYPE>(std::stoi(equipmentTypeString));
  }

  return 1;
}

int BaseConverter::HeadertoIGTL(const BaseConverter::HeaderData &header, igtl::MessageBase::Pointer *dest)
{
  (*dest)->SetDeviceName(header.deviceName.c_str());
  (*dest)->SetMetaDataElement(equipment_uid_name, IANA_TYPE_US_ASCII, header.equipmentId);
  (*dest)->SetMetaDataElement(stream_id_name, IANA_TYPE_US_ASCII, header.streamId);
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
