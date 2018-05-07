/*==============================================================================

Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
Queen's University, Kingston, ON, Canada. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
and was supported through the Applied Cancer Research Unit program of Cancer Care
Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#include "igtlioCommand.h"

// VTK includes
#include <vtkObjectFactory.h>

// IGTLIO includes
#include "igtlioCommandConverter.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioCommand);

//---------------------------------------------------------------------------
igtlioCommand::igtlioCommand()
  : CommandId(-1)
  , ClientId(-1)
  , Name("")
  , CommandContent("")
  , ResponseContent("")
  , TimeoutSec(5.0)
  , SentTimestamp(0.0)
  , CommandMetaData(igtl::MessageBase::MetaDataMap())
  , ResponseMetaData(igtl::MessageBase::MetaDataMap())
  , ErrorMessage("")
  , Direction(CommandOut)
  , Status(CommandUnknown)
{
}

//----------------------------------------------------------------------
void igtlioCommand::PrintSelf(std::ostream &os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void igtlioCommand::ClearCommandMetaData()
{
  this->CommandMetaData = igtl::MessageBase::MetaDataMap();
}

//----------------------------------------------------------------------------
igtl::MessageBase::MetaDataMap igtlioCommand::GetCommandMetaData() const
{
  return this->CommandMetaData;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetCommandMetaData(const igtl::MessageBase::MetaDataMap map)
{
  this->CommandMetaData = map;
}

//----------------------------------------------------------------------------
bool igtlioCommand::GetCommandMetaDataElement(const std::string key, std::string& outValue, IANA_ENCODING_TYPE& outEncoding)
{
  if (this->CommandMetaData.find(key) == this->CommandMetaData.end())
  {
    return false;
  }

  outEncoding = this->CommandMetaData[key].first;
  outValue = this->CommandMetaData[key].second;
  return true;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetCommandMetaDataElement(const std::string key, const std::string value, IANA_ENCODING_TYPE encoding/*=IANA_TYPE_US_ASCII*/)
{
  this->CommandMetaData[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encoding, value);
}

//----------------------------------------------------------------------------
void igtlioCommand::ClearResponseMetaData()
{
  this->ResponseMetaData = igtl::MessageBase::MetaDataMap();
}

//----------------------------------------------------------------------------
igtl::MessageBase::MetaDataMap igtlioCommand::GetResponseMetaData() const
{
  return this->ResponseMetaData;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetResponseMetaData(igtl::MessageBase::MetaDataMap map)
{
  this->ResponseMetaData = map;
  this->Modified();
}

//----------------------------------------------------------------------------
bool igtlioCommand::GetResponseMetaDataElement(const std::string key, std::string& outValue, IANA_ENCODING_TYPE& outEncoding)
{
  if (this->ResponseMetaData.find(key) == this->ResponseMetaData.end())
  {
    return false;
  }

  outEncoding = this->ResponseMetaData[key].first;
  outValue = this->ResponseMetaData[key].second;
  return true;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetResponseMetaDataElement(const std::string key, const std::string value, IANA_ENCODING_TYPE encoding/*=IANA_TYPE_US_ASCII*/)
{
  this->ResponseMetaData[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encoding, value);
  this->Modified();
}

//----------------------------------------------------------------------------
bool igtlioCommand::GetSuccessful()
{
  IANA_ENCODING_TYPE encodingType;
  std::string successString = "";
  if (!this->GetResponseMetaDataElement(igtlioCommandConverter::GetComandStatusAttributeName(), successString, encodingType))
  {
    return false;
  }
  return successString == "SUCCESS" ? true : false;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetSuccessful(bool success)
{
  this->SetResponseMetaDataElement(igtlioCommandConverter::GetComandStatusAttributeName(), success ? "SUCCESS" : "FAIL");
}

//----------------------------------------------------------------------------
std::string igtlioCommand::GetErrorMessage()
{
  IANA_ENCODING_TYPE encodingType;
  std::string messageString = "";
  this->GetResponseMetaDataElement(igtlioCommandConverter::GetComandErrorMessageAttributeName(), messageString, encodingType);
  return messageString;
}

//----------------------------------------------------------------------------
void igtlioCommand::SetErrorMessage(const std::string message)
{
  this->SetResponseMetaDataElement(igtlioCommandConverter::GetComandErrorMessageAttributeName(), message);
}

//----------------------------------------------------------------------------
std::string igtlioCommand::GetStatusAsString()
{
  switch (this->Status)
  {
  case CommandWaiting:
    return "Waiting";
  case CommandExpired:
    return "Expired";
  case CommandCancelled:
    return "Cancelled";
  case CommandFailed:
    return "Failed";
  case CommandResponseReceived:
    return "Response received";
  case CommandResponseSent:
    return "Response sent";
  case CommandUnknown:
  default:
    return "Unknown";
  }
}
