#ifndef IGTLIOCOMMANDDEVICE_H
#define IGTLIOCOMMANDDEVICE_H

#include "igtlioDevicesExport.h"
#include "igtlioCommandConverter.h"
#include "igtlioDevice.h"


typedef vtkSmartPointer<class igtlioCommandDevice> igtlioCommandDevicePointer;

/// A Device supporting the COMMAND igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT igtlioCommandDevice : public igtlioDevice
{
public:
  enum {
    CommandModifiedEvent         = 118958,
  };

  enum QUERY_STATUS {
    QUERY_STATUS_NONE,
    QUERY_STATUS_WAITING,
    QUERY_STATUS_SUCCESS,
    QUERY_STATUS_EXPIRED,
    QUERY_STATUS_ERROR,
    NUM_QUERY_STATUS,
  };

  vtkSetMacro( QueryTimeOut, double );
  vtkGetMacro( QueryTimeOut, double );

 virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioCommandConverter::ContentData content);
  igtlioCommandConverter::ContentData GetContent();
  std::vector<std::string> GetAvailableCommandNames() const;

  igtl::MessageBase::Pointer GetIGTLResponseMessage();
  igtlioCommandDevicePointer GetResponseFromCommandID(int id);

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
  igtlioDevicePointer Query;
  igtlioDevicePointer Response;
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
  static igtlioCommandDevice *New();
  vtkTypeMacro(igtlioCommandDevice, igtlioDevice);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 protected:
  igtlioCommandDevice();
  ~igtlioCommandDevice();

 protected:
  std::vector<QueryType> Queries;
  igtl::CommandMessage::Pointer OutMessage;
  igtl::RTSCommandMessage::Pointer ResponseMessage;
  igtlioCommandConverter::ContentData Content;

private:
  double QueryTimeOut;
};

//---------------------------------------------------------------------------

class OPENIGTLINKIO_DEVICES_EXPORT igtlioCommandDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioCommandDeviceCreator *New();
  vtkTypeMacro(igtlioCommandDeviceCreator,vtkObject);
};

#endif // IGTLIOCOMMANDDEVICE_H
