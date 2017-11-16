#include "igtlioBaseConverter.h"


namespace igtlio
{

//---------------------------------------------------------------------------
int BaseConverter::IGTLtoHeader(igtl::MessageBase::Pointer source, BaseConverter::HeaderData *header, igtl::MessageBase::MetaDataMap* metaInfo)
{
  header->deviceName = source->GetDeviceName();
  // get timestamp
  if (IGTLToTimestamp(source, header) == 0)
    return 0;
  igtl::MessageBase::MetaDataMap sourceMetaInfo = source->GetMetaData();
  metaInfo->clear();
  for (igtl::MessageBase::MetaDataMap::const_iterator it = sourceMetaInfo.begin(); it != sourceMetaInfo.end(); ++it)
    {
    std::string key = it->first;
    IANA_ENCODING_TYPE encodingScheme = it->second.first;
    std::string value = it->second.second;
    (*metaInfo)[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encodingScheme, value);
    }
  return 1;
}

int BaseConverter::HeadertoIGTL(const BaseConverter::HeaderData &header, igtl::MessageBase::Pointer *dest, igtl::MessageBase::MetaDataMap* metaInfo)
{
  (*dest)->SetDeviceName(header.deviceName.c_str());
  for (igtl::MessageBase::MetaDataMap::const_iterator it = metaInfo->begin(); it != metaInfo->end(); ++it)
    {
    std::string key = it->first;
    IANA_ENCODING_TYPE encodingScheme = it->second.first;
    std::string value = it->second.second;
    (*dest)->SetMetaDataElement(key, encodingScheme, value);
    }
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
