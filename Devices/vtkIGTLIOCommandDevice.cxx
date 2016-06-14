// OpenIGTLinkIF MRML includes
#include "vtkIGTLIOCommandDevice.h"

// igtl support includes
#include <igtl_util.h>
#include <igtlImageMessage.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

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
 if (Converter->fromIGTL(buffer, &HeaderData, &Content, checkCRC))
   {
   this->Modified();
   return 1;
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

 if (!Converter->toIGTL(HeaderData, Content, &this->OutMessage))
   {
   return 0;
   }

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

