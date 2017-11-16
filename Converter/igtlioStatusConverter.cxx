#include "igtlioStatusConverter.h"

#include <igtl_util.h>
#include <igtlStatusMessage.h>

namespace igtlio
{

//---------------------------------------------------------------------------
int StatusConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC, igtl::MessageBase::MetaDataMap* metaInfo)
{
  // Create a message buffer to receive  data
  igtl::StatusMessage::Pointer msg;
  msg = igtl::StatusMessage::New();
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
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, metaInfo))
    return 0;

  dest->code = msg->GetCode();
  dest->subcode = msg->GetSubCode();
  dest->errorname = msg->GetErrorName();
  dest->statusstring = msg->GetStatusString();

  return 1;
}

//---------------------------------------------------------------------------
int StatusConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::StatusMessage::Pointer* dest, igtl::MessageBase::MetaDataMap* metaInfo)
{
  if (dest->IsNull())
    *dest = igtl::StatusMessage::New();
  igtl::StatusMessage::Pointer msg = *dest;

  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  msg->SetCode(source.code);
  msg->SetSubCode(source.code);
  msg->SetErrorName(source.errorname.c_str());
  msg->SetStatusString(source.statusstring.c_str());

  msg->Pack();

  return 1;
}


} // namespace igtlio

