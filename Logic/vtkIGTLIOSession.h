#ifndef VTKIGTLIOSESSION_H
#define VTKIGTLIOSESSION_H

#include "IGTLIOUtilities.h"
#include "vtkIGTLIODevice.h"
#include "vtkIGTLIOCommandDevice.h"

#include "igtlioLogicExport.h"

class vtkMatrix4x4;
class vtkImageData;

namespace igtlio
{

typedef vtkSmartPointer<class vtkIGTLIOSession> vtkIGTLIOSessionPointer;
typedef vtkSmartPointer<class vtkIGTLIOConnector> vtkIGTLIOConnectorPointer;
typedef vtkSmartPointer<class vtkIGTLIOImageDevice> vtkIGTLIOImageDevicePointer;
typedef vtkSmartPointer<class vtkIGTLIOTransformDevice> vtkIGTLIOTransformDevicePointer;

/// Convenience interface for a single IGTL connection.
///
/// Example: Send a command to a server and get a response.
///
///     vtkIGTLIOLogicPointer root = vtkIGTLIOLogicPointer::New();
///     vtkIGTLIOSessionPointer session = root->ConnectToServer(example.org, 18333);
///
///     vtkIGTLIOCommandDevicePointer command;
///     command = session->SendCommandQuery("device_id",
///                                         "GetDeviceParameters",
///                                         "<Command><Parameter Name=\"Depth\"/></Command>",
///                                         igtlio::BLOCKING, 5);
///
///     STATUS status = command->GetStatus();
///     std::string response = command->GetResponse(); // empty on failure.
///
///
///
class OPENIGTLINKIO_LOGIC_EXPORT vtkIGTLIOSession : public vtkObject
{
public:
  /// convenience methods:

  ///
  ///  Send the given command from the given device.
  /// - If using BLOCKING, the call blocks until a response appears or timeout. Return response.
  /// - If using ASYNCHRONOUS, wait for the CommandResponseReceivedEvent event. Return device.
  ///
  vtkIGTLIOCommandDevicePointer SendCommandQuery(std::string device_id,
                                                 std::string command,
                                                 std::string content,
                                                 igtlio::SYNCHRONIZATION_TYPE synchronized = igtlio::BLOCKING,
                                                 double timeout_s = 5);
  ///
  ///  Send a command response from the given device. Asynchronous.
  /// Precondition: The given device has received a query that is not yet responded to.
  /// Return device.
  vtkIGTLIOCommandDevicePointer SendCommandResponse(std::string device_id, std::string command,
                                                    std::string content);

  ///
  ///  Send the given image from the given device. Asynchronous.
  vtkIGTLIOImageDevicePointer SendImage(std::string device_id,
                                        vtkSmartPointer<vtkImageData> image,
                                        vtkSmartPointer<vtkMatrix4x4> transform);

  /// Send the given image from the given device. Asynchronous.
  vtkIGTLIOTransformDevicePointer SendTransform(std::string device_id,
                                                vtkSmartPointer<vtkMatrix4x4> transform);

    /// TODO: add more convenience methods here.


public:
  static vtkIGTLIOSession *New();
  vtkTypeMacro(vtkIGTLIOSession, vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void StartServer(int serverPort=-1, igtlio::SYNCHRONIZATION_TYPE sync=igtlio::BLOCKING, double timeout_s=5);
  void ConnectToServer(std::string serverHost, int serverPort=-1, igtlio::SYNCHRONIZATION_TYPE sync=igtlio::BLOCKING, double timeout_s=5);

  /// Get the underlying Connector object.
  vtkIGTLIOConnectorPointer GetConnector();
  void SetConnector(vtkIGTLIOConnectorPointer connector);

private:
  vtkIGTLIOSession();

  vtkIGTLIOConnectorPointer Connector;

//  vtkIGTLIOLogicPointer remoteLogic = vtkIGTLIOLogicPointer::New();
//   vtkIGTLIOConnector connector = remoteLogic->Connect(serverHost, serverPort, CLIENT, 5.0 /* timeout in sec*/);
//   if (connector.isNull())
//   {
//     LOG_ERROR("Failed to start PlusServer");
//     exit(EXIT_FAILURE);
//   }

//   vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/);
//   std::string response = command.GetResponse();

//   vtkIGTLIOCommandPointer command = remoteLogic->SendCommandQuery("PlusServerLauncher", "<Command ... />", SYNCHRONOUS, 5.0 /* timeout in sec*/, connector);
//   std::string response = command.GetResponse();

//   vtkIGTLIOCommandPointer command = remoteConnector->SendCommandQuery("PlusServerLauncher", "<Command ... />", ASYNCHRONOUS, 5.0 /* timeout in sec*/);
//   // do some work, may check command.GetStatus() ...
//   std::string response = command.GetResponse();

  bool waitForConnection(double timeout_s);
  vtkIGTLIODevicePointer AddDeviceIfNotPresent(DeviceKeyType key);
};

} // namespace igtlio

#endif // VTKIGTLIOSESSION_H
