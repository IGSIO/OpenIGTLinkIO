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
#include "IGTLIOFixture.h"
#include "igtlioSession.h"
#include "igtlioTestUtilities.h"

///
/// Setup a client and server.
/// Send a COMMAND from client to server.
/// Server replies to the client with a RTS_COMMAND
///

int main(int argc, char **argv)
{
  ClientServerFixture fixture;

  if (!fixture.ConnectClientToServer())
    return 1;

  if (fixture.Client.Logic->GetNumberOfDevices() != 0)
  {
    std::cout << "ERROR: Client has devices before they have been added or fundamental error!" << std::endl;
    return 1;
  }

  std::cout << "*** Connection done" << std::endl;
  //---------------------------------------------------------------------------

  std::string device_name = "TestDevice";
  igtlio::CommandDevicePointer clientDevice;
  clientDevice = fixture.Client.Session->SendCommand(device_name,
                                                          "Get",
                                                          "<Command>\n"
                                                          "  <Parameter Name=\"Depth\" />\n"
                                                          "</Command>",
                                                          igtlio::ASYNCHRONOUS);

  std::cout << "*** COMMAND query sent from Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Server, igtlio::Logic::CommandReceivedEvent))
    return 1;

  std::cout << "*** COMMAND query received by Server" << std::endl;
  //---------------------------------------------------------------------------

  if(device_name != clientDevice->GetDeviceName())
  {
	  return 1;
  }

  igtlio::CommandDevicePointer serverDevice;
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

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlio::Logic::CommandResponseReceivedEvent))
    return 1;

  std::cout << "*** RTS_COMMAND response received by Client" << std::endl;
  //---------------------------------------------------------------------------

  igtlio::CommandDevice::QueryType query;
  if (!clientDevice->GetQueries().empty())
    query = clientDevice->GetQueries()[0];

  if (!igtlio::compareID(igtlio::CommandDevice::SafeDownCast(query.Query),
               igtlio::CommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Query and response dont match." << std::endl;
    return 1;
  }

  if (!igtlio::compare(serverDevice,
               igtlio::CommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Received response not equal to what the Server sent." << std::endl;
    return 1;
  }

  std::cout << "*** Client query/response match found." << std::endl;
  //---------------------------------------------------------------------------

  return 0;
}
