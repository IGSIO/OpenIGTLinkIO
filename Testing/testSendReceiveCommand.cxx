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

bool compareID(igtlio::CommandDevicePointer a, igtlio::CommandDevicePointer b)
{
  if (!a || !b)
  {
    std::cout << "FAILURE: empty device" << std::endl;
    return false;
  }

  if (a->GetContent().id != b->GetContent().id)
  {
    std::cout << "FAILURE: Command IDs dont match." << std::endl;
    return false;
  }

  return true;
}

bool compare(igtlio::CommandDevicePointer a, igtlio::CommandDevicePointer b)
{
  if (!a || !b)
  {
    std::cout << "FAILURE: empty device" << std::endl;
    return false;
  }

  if (a->GetContent().name != b->GetContent().name)
  {
    std::cout << "FAILURE: Command names dont match." << std::endl;
    return false;
  }

  if (a->GetContent().id != b->GetContent().id)
  {
    std::cout << "FAILURE: Command IDs dont match." << std::endl;
    return false;
  }

  if (a->GetContent().content != b->GetContent().content)
  {
    std::cout << "FAILURE: Command content dont match." << std::endl;
    return false;
  }

  return true;
}


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

  igtlio::CommandDevicePointer clientDevice;
  clientDevice = fixture.Client.Session->SendCommandQuery("TestDevice_Command",
                                                          "GetDeviceParameters",
                                                          "<Command>\n"
                                                          "  <Parameter Name=\"Depth\" />\n"
                                                          "</Command>",
                                                          igtlio::ASYNCHRONOUS);
  std::cout << "*** COMMAND query sent from Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Server, igtlio::vtkIGTLIOLogic::CommandQueryReceivedEvent))
    return 1;
  std::cout << "*** COMMAND query received by Server" << std::endl;
  //---------------------------------------------------------------------------

  igtlio::CommandDevicePointer serverDevice;
  serverDevice = fixture.Server.Session->SendCommandResponse(clientDevice->GetDeviceName(),
                                                             "GetDeviceParameters",
                                                             "<Command>\n"
                                                             "  <Result>GetDeviceParameters: success</Result>\n"
                                                             "  <Parameter Name=\"Depth\" Value=\"45\" />\n"
                                                             "</Command>");
  if (!serverDevice)
  {
    std::cout << "FAILURE: Server did not send response." << std::endl;
    return false;
  }
  std::cout << "*** RTS_COMMAND response sent from Server" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlio::vtkIGTLIOLogic::CommandResponseReceivedEvent))
    return 1;
  std::cout << "*** RTS_COMMAND response received by Client" << std::endl;
  //---------------------------------------------------------------------------

  igtlio::Device::QueryType query;
  if (!clientDevice->GetQueries().empty())
    query = clientDevice->GetQueries()[0];

  if (!compareID(igtlio::CommandDevice::SafeDownCast(query.Query),
               igtlio::CommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Query and response dont match." << std::endl;
    return 1;
  }

  if (!compare(serverDevice,
               igtlio::CommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Received response not equal to what the Server sent." << std::endl;
    return 1;
  }

  std::cout << "*** Client query/response match found." << std::endl;
  //---------------------------------------------------------------------------

  return 0;
}
