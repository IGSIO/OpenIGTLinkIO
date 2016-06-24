#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "vtkIGTLIOImageDevice.h"
#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "igtlImageConverter.h"
#include <vtkImageDifference.h>
#include "IGTLIOFixture.h"

bool compareID(vtkSmartPointer<vtkIGTLIOCommandDevice> a, vtkSmartPointer<vtkIGTLIOCommandDevice> b)
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

bool compare(vtkSmartPointer<vtkIGTLIOCommandDevice> a, vtkSmartPointer<vtkIGTLIOCommandDevice> b)
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

  vtkSmartPointer<vtkIGTLIOCommandDevice> clientDevice = fixture.Client.CreateDummyCommandDevice();
  fixture.Client.Connector->AddDevice(clientDevice);
  fixture.Client.Connector->SendMessage(CreateDeviceKey(clientDevice));
  std::cout << "*** COMMAND query sent from Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Server, vtkIGTLIOLogic::CommandQueryReceivedEvent))
    return 1;

  std::cout << "*** COMMAND query received by Server" << std::endl;
  //---------------------------------------------------------------------------

  vtkSmartPointer<vtkIGTLIOCommandDevice> serverDevice;
  serverDevice = vtkIGTLIOCommandDevice::SafeDownCast(fixture.Server.Logic->GetDevice(0));
  fixture.Server.ConvertCommandDeviceToResponse(serverDevice);

  DeviceKeyType serverDeviceKey = CreateDeviceKey(serverDevice);
  fixture.Server.Connector->SendMessage(serverDeviceKey, vtkIGTLIODevice::MESSAGE_PREFIX_REPLY);
  std::cout << "*** RTS_COMMAND response sent from Server" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, vtkIGTLIOLogic::CommandResponseReceivedEvent))
    return 1;

  std::cout << "*** RTS_COMMAND response received by Client" << std::endl;
  //---------------------------------------------------------------------------

  vtkIGTLIODevice::QueryType query;
  if (!clientDevice->GetQueries().empty())
    query = clientDevice->GetQueries()[0];

  if (!compareID(vtkIGTLIOCommandDevice::SafeDownCast(query.Query),
               vtkIGTLIOCommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Query and response dont match." << std::endl;
    return 1;
  }

  if (!compare(serverDevice,
               vtkIGTLIOCommandDevice::SafeDownCast(query.Response)))
  {
    std::cout << "FAILURE: Received response not equal to what the Server sent." << std::endl;
    return 1;
  }

  std::cout << "*** Client query/response match found." << std::endl;
  //---------------------------------------------------------------------------

  return 0;
}
