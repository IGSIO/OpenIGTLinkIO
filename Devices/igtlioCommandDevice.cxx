#include "igtlioCommandDevice.h"

#include <vtkObjectFactory.h>
#include <vtkTimerLog.h>

namespace  igtlio
{

//---------------------------------------------------------------------------
DevicePointer CommandDeviceCreator::Create(std::string device_name)
{
 CommandDevicePointer retval = CommandDevicePointer::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string CommandDeviceCreator::GetDeviceType() const
{
 return CommandConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(CommandDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(CommandDevice);
//---------------------------------------------------------------------------
CommandDevice::CommandDevice()
{
	QueryTimeOut = 0;
}

//---------------------------------------------------------------------------
CommandDevice::~CommandDevice()
{
}

//---------------------------------------------------------------------------
unsigned int CommandDevice::GetDeviceContentModifiedEvent() const
{
  return CommandModifiedEvent;
}
  
//---------------------------------------------------------------------------
std::string CommandDevice::GetDeviceType() const
{
  return CommandConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int CommandDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  // RTS_COMMAND received:
  //    - look in the query queue for anyone waiting for it.
  if (buffer->GetDeviceType()==std::string(CommandConverter::GetIGTLResponseName()))
	{
	CommandDevicePointer response = CommandDevicePointer::New();
	if (!CommandConverter::fromIGTLResponse(buffer, &response->HeaderData, &response->Content, checkCRC, &this->metaInfo))
	  return 0;

	// search among the queries for a command with an identical ID:
	for (unsigned i=0; i<Queries.size(); ++i)
	  {
	  CommandDevicePointer query = CommandDevice::SafeDownCast(Queries[i].Query.GetPointer());
	  if (query && query->GetContent().id == response->GetContent().id)
		{
		Queries[i].Response = response;
		Queries[i].status = QUERY_STATUS_SUCCESS;
		this->Modified();
		this->InvokeEvent(CommandResponseReceivedEvent, this);
		}
	  }

	return 1;
	}

  // COMMAND received
  //   - store the incoming message, emit event
  //     No response is created - this is the responsibility of the application.
  if (buffer->GetDeviceType()==std::string(CommandConverter::GetIGTLTypeName()))
	{
	if (CommandConverter::fromIGTL(buffer, &HeaderData, &Content, checkCRC, &this->metaInfo))
	  {
	  this->Modified();
	  this->InvokeEvent(CommandReceivedEvent, this);
	  return 1;
	  }
	}

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer CommandDevice::GetIGTLMessage()
{
 // cannot send a non-existent Command (?)
 if (Content.name.empty())
  {
  return 0;
  }

 this->SetTimestamp(vtkTimerLog::GetUniversalTime());

 if (!CommandConverter::toIGTL(HeaderData, Content, &this->OutMessage, &this->metaInfo))
   {
   return 0;
   }


 // store the current device state as a query
 QueryType query;
 CommandDevicePointer queryDevice = CommandDevicePointer::New();
 queryDevice->SetContent(this->GetContent());
 queryDevice->SetHeader(this->GetHeader()); // NOTE: requires timestamp to be current
 query.Query = queryDevice;
 query.status = QUERY_STATUS_WAITING;
 Queries.push_back(query);

 // Store copy of current content/id in query buffer, waiting for reply.
 // When reply arrives (via ReceiveIGTLMessage), store as a pair in a separate
 // response-list. Emit signals to notify that responses have been received.


 return dynamic_pointer_cast<igtl::MessageBase>(this->OutMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer CommandDevice::GetIGTLResponseMessage()
{
 // cannot send a non-existent Command (?)
 if (Content.name.empty())
  {
  return 0;
  }

 this->SetTimestamp(vtkTimerLog::GetUniversalTime());

 if (this->ResponseMessage.IsNull())
   this->ResponseMessage = igtl::RTSCommandMessage::New();

 igtl::CommandMessage::Pointer response = dynamic_pointer_cast<igtl::CommandMessage>(this->ResponseMessage);
 if (!CommandConverter::toIGTL(HeaderData, Content, &response, &this->metaInfo))
   {
   return 0;
   }

 return dynamic_pointer_cast<igtl::MessageBase>(this->ResponseMessage);
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer CommandDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();
   }
  if (prefix==Device::MESSAGE_PREFIX_RTS)
   {
     return this->GetIGTLResponseMessage();
   }


 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<Device::MESSAGE_PREFIX> CommandDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 retval.insert(MESSAGE_PREFIX_RTS);
 return retval;
}

void CommandDevice::SetContent(CommandConverter::ContentData content)
{
  Content = content;
  this->Modified();
  this->InvokeEvent(CommandModifiedEvent, this);
}

CommandConverter::ContentData CommandDevice::GetContent()
{
  return Content;
}

std::vector<std::string> CommandDevice::GetAvailableCommandNames() const
{
  return CommandConverter::GetAvailableCommandNames();
}

//---------------------------------------------------------------------------
void CommandDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  Device::PrintSelf(os, indent);

  os << indent << "CommandID:\t" << Content.id << "\n";
  os << indent << "CommandName:\t" << Content.name << "\n";
  os << indent << "CommandContent:\t" << "\n";
  os << Content.content << "\n";
}

//---------------------------------------------------------------------------
CommandDevicePointer CommandDevice::GetResponseFromCommandID(int id)
{
  // search among the queries for a command with an identical ID:
  for (unsigned i=0; i<Queries.size(); ++i)
  {
	CommandDevicePointer response = CommandDevice::SafeDownCast(Queries[i].Response);
    if (response && response->GetContent().id == id)
    {
      return response;
    }
  }

  return CommandDevicePointer();
}
//---------------------------------------------------------------------------
std::vector<CommandDevice::QueryType> CommandDevice::GetQueries() const
{
  return Queries;
}

//---------------------------------------------------------------------------
int CommandDevice::CheckQueryExpiration()
{
  double currentTime = vtkTimerLog::GetUniversalTime();
//  if (this->QueryWaitingQueue.size() > 0)
//    {
//    for (std::list< vtkWeakPointer<vtkMRMLIGTLQueryNode> >::iterator iter = this->QueryWaitingQueue.begin();
//      iter != this->QueryWaitingQueue.end(); /* increment in the loop to allow erase */ )
//      {
//      if (iter->GetPointer()==NULL)
//        {
//        // the node has been deleted, so remove it from the list
//        iter = this->QueryWaitingQueue.erase(iter);
//        continue;
//        }
  bool expired = false;

  for (unsigned i=0; i<Queries.size(); ++i)
	{
	  double timeout = this->GetQueryTimeOut();
	  if ((timeout>0)
		  && (currentTime-Queries[i].Query->GetTimestamp()>timeout)
		  && (Queries[i].status==QUERY_STATUS_WAITING))
		{
		Queries[i].status=QUERY_STATUS_EXPIRED;
		expired = true;
		}

	}

  //Never used? Why a responsevent???
  if (expired)
	this->InvokeEvent(ResponseEvent);

  return 0;
}

//---------------------------------------------------------------------------
int CommandDevice::PruneCompletedQueries()
{
  std::vector<QueryType> pruned;

  for (unsigned int i=0; i<Queries.size(); ++i)
	if (Queries[i].status == QUERY_STATUS_WAITING)
	  pruned.push_back(Queries[i]);

  if (pruned.size()!=Queries.size())
	this->Modified();

  Queries = pruned;
  return 0;
}

//---------------------------------------------------------------------------
int CommandDevice::CancelQuery(int index)
{
  Queries.erase(Queries.begin()+index);
  return 0;
}

} // namespace igtlio
