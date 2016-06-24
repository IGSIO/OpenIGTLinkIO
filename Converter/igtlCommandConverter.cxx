#include "igtlCommandConverter.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlCommandMessage.h>

// VTK includes
#include <vtkVersion.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>


namespace igtl
{

CommandConverter::CommandConverter()
{
}

//---------------------------------------------------------------------------
CommandConverter::~CommandConverter()
{
}

//---------------------------------------------------------------------------
void CommandConverter::PrintSelf(std::ostream &os) const
{
 this->BaseConverter::PrintSelf(os);
}

std::vector<std::string> CommandConverter::GetAvailableCommandNames() const
{
  std::vector<std::string> retval;
  retval.push_back("SetDeviceParameters");
  retval.push_back("GetDeviceParameters");
  retval.push_back("SubscribeDeviceParameters");
  retval.push_back("GetCapabilities");
  return retval;
}

//---------------------------------------------------------------------------
int CommandConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC)
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
  if (!this->IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header))
    return 0;

  dest->id = msg->GetCommandId();
  dest->name = msg->GetCommandName();
  dest->content = msg->GetCommandContent();

  return 1;
}

int CommandConverter::fromIGTLResponse(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC)
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
  if (!this->IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header))
    return 0;

  dest->id = msg->GetCommandId();
  dest->name = msg->GetCommandName();
  dest->content = msg->GetCommandContent();

  return 1;
}


//---------------------------------------------------------------------------
int CommandConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::CommandMessage::Pointer* dest)
{
  if (dest->IsNull())
    *dest = igtl::CommandMessage::New();
  igtl::CommandMessage::Pointer msg = *dest;

  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  this->HeadertoIGTL(header, &basemsg);

  msg->SetCommandId(source.id);
  msg->SetCommandName(source.name);
  msg->SetCommandContent(source.content);

  msg->Pack();

  return 1;
}


}

