#ifndef IGTLIOCOMMANDDEVICE_H
#define IGTLIOCOMMANDDEVICE_H

#include "igtlioDevicesExport.h"
#include "igtlioCommandConverter.h"
#include "igtlioDevice.h"

namespace igtlio
{
typedef vtkSmartPointer<class CommandDevice> CommandDevicePointer;

/// A Device supporting the COMMAND igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT CommandDevice : public Device
{
public:
	enum QUERY_STATUS {
	  QUERY_STATUS_NONE,
	  QUERY_STATUS_WAITING,
	  QUERY_STATUS_SUCCESS,
	  QUERY_STATUS_EXPIRED,
	  QUERY_STATUS_ERROR,
	  NUM_QUERY_STATUS,
	};

	enum {
	  CommandModifiedEvent         = 118958,
	};

public:
  vtkSetMacro( QueryTimeOut, double );
  vtkGetMacro( QueryTimeOut, double );

  virtual vtkIntArray* GetDeviceContentModifiedEvent() const;
  virtual std::string GetDeviceType() const;
  virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
  virtual igtl::MessageBase::Pointer GetIGTLMessage();
  virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
  virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(CommandConverter::ContentData content);
  CommandConverter::ContentData GetContent();
  std::vector<std::string> GetAvailableCommandNames() const;

  igtl::MessageBase::Pointer GetIGTLResponseMessage();
  CommandDevicePointer GetResponseFromCommandID(int id);

  /// Query handling:
  /// Each device has a list of queries (GET_, STT_, STP_) that has been sent
  /// and are awaiting reply.
  //
  /// Device::GetMessage() pushes a query,
  /// Device::ReceiveMessage() processes the reply, and emits events for the receive
  ///   - statechange: waiting, success, expired,...
  ///
  /// One query (GET_, STT_ or STP_-message that requires an answer)
  ///
  /// TODO: Currently implemented for COMMAND message only. The GET_/STT_/STP_ messages
  /// handle this by simply sending and ignoring failures.
  /// Either move the query mechanism down to COMMAND or generalize.
  ///
  ///
  struct QueryType
  {
	DevicePointer Query;
	DevicePointer Response;
	QUERY_STATUS status;
  };

   /// Get all current queries
   std::vector<QueryType> GetQueries() const;

   /// check for waiting queries that have waited beoynd the timeout for an answer, mark them as expired.

   int CheckQueryExpiration();
   /// remove all queries that are answered or expired.
   int PruneCompletedQueries();

   int CancelQuery(int index);

 public:
  static CommandDevice *New();
  vtkTypeMacro(CommandDevice,Device);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  CommandDevice();
  ~CommandDevice();

 protected:
  std::vector<QueryType> Queries;
  igtl::CommandMessage::Pointer OutMessage;
  igtl::RTSCommandMessage::Pointer ResponseMessage;
  CommandConverter::ContentData Content;

private:
  double QueryTimeOut;
};

//---------------------------------------------------------------------------

class OPENIGTLINKIO_DEVICES_EXPORT CommandDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static CommandDeviceCreator *New();
  vtkTypeMacro(CommandDeviceCreator,vtkObject);
};

} // namespace igtlio

#endif // IGTLIOCOMMANDDEVICE_H
