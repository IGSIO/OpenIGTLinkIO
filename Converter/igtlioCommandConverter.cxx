#include "igtlioCommandConverter.h"

// STL includes
#include <algorithm>
#include <locale>

std::vector<std::string> igtlioCommandConverter::GetAvailableCommandNames()
{
  std::vector<std::string> retval;
  retval.push_back("Set");
  retval.push_back("Get");
  retval.push_back("Subscribe");
  retval.push_back("Unsubscribe");
  return retval;
}

//---------------------------------------------------------------------------
int igtlioCommandConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC, 
                             igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Create a message buffer to receive  data
  igtl::CommandMessage::Pointer msg;
  msg = igtl::CommandMessage::New();
  msg->Copy(source); // !! TODO: copy makes performance issue.

  // Deserialize the data
  // If CheckCRC==0, CRC check is skipped.
  int c = msg->Unpack(checkCRC);

  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, outMetaInfo))
    return 0;

  dest->id = msg->GetCommandId();
  dest->name = msg->GetCommandName();
  dest->content = msg->GetCommandContent();

  return 1;
}

int igtlioCommandConverter::fromIGTLResponse(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC,
                             igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  //TODO: merge this method with fromIGTL(),

  // Create a message buffer to receive  data
  igtl::RTSCommandMessage::Pointer msg;
  msg = igtl::RTSCommandMessage::New();
  msg->Copy(source); // !! TODO: copy makes performance issue.

  // Deserialize the data
  // If CheckCRC==0, CRC check is skipped.
  int c = msg->Unpack(checkCRC);

  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
    // TODO: error handling
    return 0;
    }

  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, outMetaInfo))
    return 0;

  dest->id = msg->GetCommandId();
  dest->name = msg->GetCommandName();
  dest->content = msg->GetCommandContent();
  std::string status;
  if (msg->GetMetaDataElement("Status", status))
  {
    std::transform(status.begin(), status.end(), status.begin(), toupper);
    dest->status = status == "TRUE";
  }

  return 1;
}


//---------------------------------------------------------------------------
int igtlioCommandConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::CommandMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
    *dest = igtl::CommandMessage::New();
  (*dest)->InitPack();
  igtl::CommandMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
    {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  msg->SetCommandId(source.id);
  msg->SetCommandName(source.name);
  msg->SetCommandContent(source.content);
  msg->SetMetaDataElement("Status", IANA_TYPE_US_ASCII, source.status ? "SUCCESS" : "FAIL");
  msg->Pack();

  return 1;
}

//---------------------------------------------------------------------------
int igtlioCommandConverter::toIGTLResponse(const HeaderData& header, const ContentData& source, igtl::RTSCommandMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
    *dest = igtl::RTSCommandMessage::New();
  (*dest)->InitPack();
  igtl::RTSCommandMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
    {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  msg->SetCommandId(source.id);
  msg->SetCommandName(source.name);
  msg->SetCommandContent(source.content);
  msg->SetMetaDataElement("Status", IANA_TYPE_US_ASCII, source.status ? "SUCCESS" : "FAIL");
  msg->Pack();

  return 1;
}
