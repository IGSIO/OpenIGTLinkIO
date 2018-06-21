#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include "igtlioImageDevice.h"
#include "vtkImageData.h"
#include "igtlioImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "igtlioImageConverter.h"
#include <vtkImageDifference.h>
#include "igtlioFixture.h"
#include "igtlioSession.h"
#include "igtlioTestUtilities.h"

///
/// Setup a client and server.
/// Send a COMMAND from client to server.
/// Server replies to the client with a RTS_COMMAND
///
igtlioCommandPointer command = NULL;
void testCommandReceived(vtkObject* vtkNotUsed(object), unsigned long event, void *vtkNotUsed(clientdata), void* (calldata))
{
  command = reinterpret_cast<igtlioCommand*>(calldata);
}

int main(int argc, char **argv)
{
  igtlioClientServerFixture fixture;

  if (!fixture.ConnectClientToServer())
    return TEST_FAILED;

  if (fixture.Client.Logic->GetNumberOfDevices() != 0)
  {
    std::cout << "ERROR: Client has devices before they have been added or fundamental error!" << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** Connection done" << std::endl;
  //---------------------------------------------------------------------------

  vtkSmartPointer<vtkCallbackCommand> commandReceivedCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  commandReceivedCallback->SetCallback(testCommandReceived);
  fixture.Server.Connector->AddObserver(igtlioCommand::CommandReceivedEvent, commandReceivedCallback);

  std::string device_name = "TestDevice";
  igtlioCommandPointer sentCommand;
  sentCommand = fixture.Client.Session->SendCommand("Get",
                                                    "<Command>\n"
                                                    "  <Parameter Name=\"Depth\" />\n"
                                                    "</Command>",
                                                    IGTLIO_ASYNCHRONOUS);

  fixture.LoopUntilEventDetected(&fixture.Server, igtlioCommand::CommandReceivedEvent);

  if (!sentCommand || sentCommand->GetStatus() != igtlioCommandStatus::CommandWaiting)
  {
    std::cout << "FAILURE: Command not sent." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** COMMAND sent from Client" << std::endl;
  //---------------------------------------------------------------------------

  igtlioCommandPointer receivedCommand = command;
  if (!receivedCommand || sentCommand->GetName() != receivedCommand->GetName())
  {
    std::cout << "FAILURE: Command names in server and client do not match." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** COMMAND query received by Server" << std::endl;
  //---------------------------------------------------------------------------

  receivedCommand->SetResponseContent("<Command>\n"
                                      "  <Result success=true> <Parameter Name=”Depth” /> </Result>\n"
                                      "</Command>");
  receivedCommand->SetSuccessful(true);

  int success = fixture.Server.Session->SendCommandResponse(receivedCommand);
  if (!success)
  {
    std::cout << "FAILURE: Server did not send response." << std::endl;
    return false;
  }

  std::cout << "*** RTS_COMMAND response sent from Server" << std::endl;
  //---------------------------------------------------------------------------

  fixture.LoopUntilEventDetected(&fixture.Client, igtlioCommand::CommandResponseEvent);

  if (sentCommand->GetStatus() != igtlioCommandStatus::CommandResponseReceived)
  {
    std::cout << "FAILURE: Response sent but not received." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** RTS_COMMAND response received by Client" << std::endl;
  //---------------------------------------------------------------------------

  if (sentCommand->GetResponseContent() != receivedCommand->GetResponseContent())
  {
    std::cout << "FAILURE: Received response not equal to what the Server sent." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** Client command/response match found." << std::endl;
  //---------------------------------------------------------------------------

  return TEST_SUCCESS;
}
