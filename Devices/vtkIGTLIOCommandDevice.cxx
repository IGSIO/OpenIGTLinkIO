// OpenIGTLinkIF MRML includes
#include "vtkIGTLIOCommandDevice.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>
#include "igtlImageConverter.h"

//---------------------------------------------------------------------------
vtkSmartPointer<vtkIGTLIODevice> vtkIGTLIOCommandDeviceCreator::Create(std::string device_name)
{
 vtkSmartPointer<vtkIGTLIOCommandDevice> retval = vtkSmartPointer<vtkIGTLIOCommandDevice>::New();
 retval->SetDeviceName(device_name);
 return retval;
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOCommandDeviceCreator::GetDeviceType() const
{
 return igtl::CommandConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOCommandDeviceCreator);




//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOCommandDevice);
//---------------------------------------------------------------------------
vtkIGTLIOCommandDevice::vtkIGTLIOCommandDevice()
{
   Converter = igtl::CommandConverter::New();
}

//---------------------------------------------------------------------------
vtkIGTLIOCommandDevice::~vtkIGTLIOCommandDevice()
{
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOCommandDevice::GetDeviceType() const
{
  return igtl::CommandConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
int vtkIGTLIOCommandDevice::ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC)
{
  // If we receive a RTS_COMMAND, look in the query queue for anyone waiting for it.
  //
  // TODO: If we receive a COMMAND, we are expected to act as a server
  // and respond to the COMMAND contents.

  if (buffer->GetDeviceType()==std::string(Converter->GetIGTLResponseName()))
    {
    vtkSmartPointer<vtkIGTLIOCommandDevice> response = vtkSmartPointer<vtkIGTLIOCommandDevice>::New();
    if (!Converter->fromIGTL(buffer, &response->HeaderData, &response->Content, checkCRC))
      return 0;

    // search among the queries for a command with an identical ID:
    for (unsigned i=0; i<Queries.size(); ++i)
      {
      vtkSmartPointer<vtkIGTLIOCommandDevice> query = vtkIGTLIOCommandDevice::SafeDownCast(Queries[i].Query.GetPointer());
      if (query && query->GetContent().id == response->GetContent().id)
        {
        Queries[i].Response = response;
        this->Modified();
        std::cout << "stored response: \n";
        response->Print(std::cout);
        }
      }

    return 1;
    }


  if (buffer->GetDeviceType()==std::string(Converter->GetIGTLTypeName()))
    {
    vtkErrorMacro ("Client does not handle incoming COMMAND messages\n");
    return 0;
    }

 return 0;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkIGTLIOCommandDevice::GetIGTLMessage()
{
 // cannot send a non-existent Command (?)
 if (Content.name.empty())
  {
  return 0;
  }

 this->SetTimestamp(vtkTimerLog::GetUniversalTime());

 if (!Converter->toIGTL(HeaderData, Content, &this->OutMessage))
   {
   return 0;
   }


 // store the current device state as a query
 QueryType query;
 vtkSmartPointer<vtkIGTLIOCommandDevice> queryDevice = vtkSmartPointer<vtkIGTLIOCommandDevice>::New();
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
igtl::MessageBase::Pointer vtkIGTLIOCommandDevice::GetIGTLMessage(MESSAGE_PREFIX prefix)
{
  if (prefix==MESSAGE_PREFIX_NOT_DEFINED)
   {
     return this->GetIGTLMessage();

   }

 return igtl::MessageBase::Pointer();
}

//---------------------------------------------------------------------------
std::set<vtkIGTLIODevice::MESSAGE_PREFIX> vtkIGTLIOCommandDevice::GetSupportedMessagePrefixes() const
{
 std::set<MESSAGE_PREFIX> retval;
 return retval;
}

void vtkIGTLIOCommandDevice::SetContent(igtl::CommandConverter::ContentData content)
{
  Content = content;
  this->Modified();
}

igtl::CommandConverter::ContentData vtkIGTLIOCommandDevice::GetContent()
{
  return Content;
}

std::vector<std::string> vtkIGTLIOCommandDevice::GetAvailableCommandNames() const
{
  return Converter->GetAvailableCommandNames();
}

//---------------------------------------------------------------------------
void vtkIGTLIOCommandDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkIGTLIODevice::PrintSelf(os, indent);

  os << indent << "CommandID:\t" << Content.id << "\n";
  os << indent << "CommandName:\t" << Content.name << "\n";
  os << indent << "CommandContent:\t" << "\n";
  os << Content.content << "\n";
}

