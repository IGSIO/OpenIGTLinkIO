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

  vtkSmartPointer<vtkIGTLIOCommandDevice> commandDevice = fixture.Client.CreateDummyCommandDevice();
  fixture.Client.Connector->AddDevice(commandDevice);
  fixture.Client.Connector->SendMessage(CreateDeviceKey(commandDevice));
  std::cout << "*** COMMAND sent from Client" << std::endl;

  std::cout << "Client fixture: " << &fixture.Client << std::endl;
  std::cout << "Server fixture: " << &fixture.Server << std::endl;

  if (!fixture.LoopUntilEventDetected(&fixture.Server, vtkIGTLIOLogic::CommandQueryReceivedEvent))
    return 1;
//  bool ClientServerFixture::LoopUntilEventDetected(LogicFixture* logic, int eventId)

//  if (!fixture.LoopUntilExpectedNumberOfDevicesReached(fixture.Server, 1))
//    return 1;

  std::cout << "*** COMMAND query received by Server" << std::endl;

  vtkSmartPointer<vtkIGTLIOCommandDevice> serverDevice;
  serverDevice = vtkIGTLIOCommandDevice::SafeDownCast(fixture.Server.Logic->GetDevice(0));
  if (!serverDevice)
  {
    std::cout << "FAILURE: Non-command device received." << std::endl;
    return 1;
  }

  igtl::CommandConverter::ContentData content = serverDevice->GetContent();
  if (content.name != "GetDeviceParameters")
  {
    std::cout << "FAILURE: Wrong command received." << std::endl;
    return 1;
  }

  content.content = ""
      "<Command>\n"
      "  <Result>GetDeviceParameters: success</Result>\n"
      "  <Parameter Name=\"Depth\" Value=\"45\" />\n"
      "</Command>";

  serverDevice->SetContent(content);

  DeviceKeyType serverDeviceKey = CreateDeviceKey(serverDevice);
  fixture.Server.Connector->SendMessage(serverDeviceKey, vtkIGTLIODevice::MESSAGE_PREFIX_REPLY);
  std::cout << "*** RTS_COMMAND sent from Server" << std::endl;

  if (!fixture.LoopUntilEventDetected(&fixture.Client, vtkIGTLIOLogic::CommandResponseReceivedEvent))
    return 1;

  std::cout << "*** COMMAND response received by Client" << std::endl;

  return 0;
}
