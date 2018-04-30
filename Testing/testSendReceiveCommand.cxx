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

  std::string device_name = "TestDevice";
  igtlioCommandDevicePointer clientDevice;
  clientDevice = fixture.Client.Session->SendCommand(device_name,
                                                          "Get",
                                                          "<Command>\n"
                                                          "  <Parameter Name=\"Depth\" />\n"
                                                          "</Command>",
                                                          IGTLIO_ASYNCHRONOUS);

  std::cout << "*** COMMAND query sent from Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Server, igtlioLogic::CommandReceivedEvent))
    return TEST_FAILED;

  std::cout << "*** COMMAND query received by Server" << std::endl;
  //---------------------------------------------------------------------------

  if(device_name != clientDevice->GetDeviceName())
  {
    return TEST_FAILED;
  }

  igtlioCommandDevicePointer serverDevice;
  serverDevice = fixture.Server.Session->SendCommandResponse(clientDevice->GetDeviceName(),
                                                             "Get",
                                                             "<Command>\n"
                                                             "  <Result success=true> <Parameter Name=”Depth” /> </Result>\n"
                                                             "</Command>");
  if (!serverDevice)
  {
    std::cout << "FAILURE: Server did not send response." << std::endl;
    return false;
  }

  std::cout << "*** RTS_COMMAND response sent from Server" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::CommandResponseReceivedEvent))
    return TEST_FAILED;

  std::cout << "*** RTS_COMMAND response received by Client" << std::endl;
  //---------------------------------------------------------------------------

  igtlioCommandDevice::QueryType query;
  if (!clientDevice->GetQueries().empty())
    query = clientDevice->GetQueries()[0];

  if (!compareID(igtlioCommandDevice::SafeDownCast(query.Query),
               igtlioCommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Query and response dont match." << std::endl;
    return TEST_FAILED;
  }

  if (!compare(serverDevice,
               igtlioCommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Received response not equal to what the Server sent." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** Client query/response match found." << std::endl;
  //---------------------------------------------------------------------------

  return TEST_SUCCESS;
}
