#include "igtlioStringConverter.h"

#include <igtl_util.h>
#include <igtlStringMessage.h>

namespace igtlio
{

//---------------------------------------------------------------------------
int StringConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC)
{
  // Create a message buffer to receive  data
  igtl::StringMessage::Pointer msg;
  msg = igtl::StringMessage::New();
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
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header))
    return 0;

  dest->string_msg = msg->GetString();
  dest->encoding = msg->GetEncoding();

  return 1;
}

//---------------------------------------------------------------------------
int StringConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::StringMessage::Pointer* dest)
{
  if (dest->IsNull())
    *dest = igtl::StringMessage::New();
  igtl::StringMessage::Pointer msg = *dest;

  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg);

  msg->SetString(source.string_msg);
  msg->SetEncoding(source.encoding);

  msg->Pack();

  return 1;
}


} // namespace igtlio


