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

#ifndef igtlioCommand_H
#define igtlioCommand_H

#include "igtlioLogicExport.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// IGTL includes
#include <igtlCommandMessage.h>

typedef vtkSmartPointer<class igtlioCommand> igtlioCommandPointer;

enum igtlioCommandStatus
{
  CommandUnknown,
  CommandWaiting,
  CommandExpired,
  CommandCancelled,
  CommandFailed,
  CommandResponseReceived,
  CommandResponseSent
};

enum igtlioCommandDirection
{
  CommandIn,
  CommandOut,
};

class OPENIGTLINKIO_LOGIC_EXPORT igtlioCommand : public vtkObject
{
public:
  static igtlioCommand *New();
  vtkTypeMacro(igtlioCommand, vtkObject);
  void PrintSelf(ostream&, vtkIndent) VTK_OVERRIDE;

private:
  igtlioCommand();

public:

  enum
  {
    CommandResponseEvent = 128000,
    CommandExpiredEvent = 128001,
    CommandReceivedEvent = 128002,
    CommandCancelledEvent = 128003,
    CommandCompletedEvent = 128004, // Invoked for all Expired/Cancelled/Response events
  };

  // Command info
  vtkSetMacro(CommandId, int);
  vtkGetMacro(CommandId, int);
  vtkSetMacro(ClientId, int);
  vtkGetMacro(ClientId, int);

  // Command content
  vtkSetMacro(Name, std::string);
  vtkGetMacro(Name, std::string);
  vtkSetMacro(CommandContent, std::string);
  vtkGetMacro(CommandContent, std::string);

  void ClearCommandMetaData();
  igtl::MessageBase::MetaDataMap GetCommandMetaData() const;
  void SetCommandMetaData(const igtl::MessageBase::MetaDataMap map);
  bool GetCommandMetaDataElement(const std::string key, std::string& outValue, IANA_ENCODING_TYPE& outEncoding);
  void SetCommandMetaDataElement(const std::string key, const std::string value, IANA_ENCODING_TYPE encoding = IANA_TYPE_US_ASCII);
 

  // Response content
  vtkSetMacro(ResponseContent, std::string);
  vtkGetMacro(ResponseContent, std::string);

  void ClearResponseMetaData();
  igtl::MessageBase::MetaDataMap GetResponseMetaData() const;
  bool GetResponseMetaDataElement(const std::string key, std::string& outValue, IANA_ENCODING_TYPE& outEncoding);
  void SetResponseMetaData(const igtl::MessageBase::MetaDataMap map);
  void SetResponseMetaDataElement(const std::string key, const std::string value, IANA_ENCODING_TYPE encoding = IANA_TYPE_US_ASCII);

  // Communication parameters
  vtkSetMacro(TimeoutSec, double);
  vtkGetMacro(TimeoutSec, double);
  vtkSetMacro(SentTimestamp, double);
  vtkGetMacro(SentTimestamp, double);
  vtkBooleanMacro(Blocking, bool);
  vtkSetMacro(Blocking, bool);
  vtkGetMacro(Blocking, bool);

  // Direction of the command
  vtkGetMacro(Direction, int);
  vtkSetMacro(Direction, int);
  void SetDirectionIn() { this->Direction = CommandIn; };
  void SetDirectionOut() { this->Direction = CommandOut; };
  bool IsDirectionIn() { return this->Direction == CommandIn; };
  bool IsDirectionOut() { return this->Direction == CommandOut; };

  // Communication status information of the command
  vtkSetMacro(Status, igtlioCommandStatus);
  vtkGetMacro(Status, igtlioCommandStatus);
  bool IsInProgress() { return this->Status == CommandWaiting; };
  bool IsCompleted() { return !this->IsInProgress() && this->Status != CommandUnknown; };
  std::string GetStatusAsString();

  // Status information on how the command was handled
  bool GetSuccessful();
  void SetSuccessful(bool success);

  std::string GetErrorMessage();
  void SetErrorMessage(const std::string message);

private:

  // Command info
  int                             CommandId;
  int                             ClientId;

  // Command content
  std::string                     Name;
  std::string                     CommandContent;
  igtl::MessageBase::MetaDataMap  CommandMetaData;

  // Response content
  std::string                     ResponseContent;
  igtl::MessageBase::MetaDataMap  ResponseMetaData;

  // Communication parameters
  double                          TimeoutSec;
  double                          SentTimestamp;
  bool                            Blocking;

  // Communication status information of the command
  // Ex. Waiting/Cancelled/Expired/Received Response/Sent Response
  igtlioCommandStatus             Status;

  // Information on how the command was handled
  // Ex. Command response was received, but there was some issue executing the command instructions
  std::string                     ErrorMessage;

  // If the command is incoming or outgoing
  int Direction;

};

#endif // igtlioCommand_H