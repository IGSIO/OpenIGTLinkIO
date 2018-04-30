#include "igtlioBaseConverter.h"

//---------------------------------------------------------------------------
int igtlioBaseConverter::IGTLtoHeader(igtl::MessageBase::Pointer source, HeaderData *header, igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  header->deviceName = source->GetDeviceName();
  // get timestamp
  if (IGTLToTimestamp(source, header) == 0)
    return 0;

  igtl::MessageBase::MetaDataMap sourceMetaInfo = source->GetMetaData();
  outMetaInfo.clear();
  for (igtl::MessageBase::MetaDataMap::const_iterator it = sourceMetaInfo.begin(); it != sourceMetaInfo.end(); ++it)
    {
    std::string key = it->first;
    IANA_ENCODING_TYPE encodingScheme = it->second.first;
    std::string value = it->second.second;
    outMetaInfo[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encodingScheme, value);
    }
  return 1;
}

//---------------------------------------------------------------------------
int igtlioBaseConverter::HeadertoIGTL(const HeaderData &header, igtl::MessageBase::Pointer *dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  (*dest)->SetDeviceName(header.deviceName.c_str());
  for (igtl::MessageBase::MetaDataMap::const_iterator it = metaInfo.begin(); it != metaInfo.end(); ++it)
    {
    std::string key = it->first;
    IANA_ENCODING_TYPE encodingScheme = it->second.first;
    std::string value = it->second.second;
    (*dest)->SetMetaDataElement(key, encodingScheme, value);
    }
  return 1;
}

//---------------------------------------------------------------------------
int igtlioBaseConverter::IGTLToTimestamp(igtl::MessageBase::Pointer msg, HeaderData *dest)
{
  // Save OpenIGTLink time stamp
  igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
  msg->GetTimeStamp(ts);
  dest->timestamp = ts->GetTimeStamp();
  return 1;
}
