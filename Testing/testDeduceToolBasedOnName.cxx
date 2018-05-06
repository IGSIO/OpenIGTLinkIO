#include "igtlioFixture.h"
#include "igtlioSession.h"
#include "igtlioTransformDevice.h"
#include "igtlioTestUtilities.h"

///
/// Setup a client and server.
/// Simulate a server sending:
///      IMAGE and TRANSFROM from a usprobe
///      TRANSFORM from a pointer
/// Receive messages on the client.
/// Deduce tools (usprobe and pointer) base on incoming messages.
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

  std::string usprobe_name = "usprobe";
  std::string usprobe_image_stream_name = usprobe_name+"_image";
  std::string usprobe_transform_stream_name = usprobe_name+"_transform";

  igtlioImageDevicePointer imageDevice;
  imageDevice = fixture.Server.Session->SendImage(usprobe_image_stream_name,
                                                  fixture.CreateTestImage(),
                                                  fixture.CreateTestTransform());

  igtlioTransformDevicePointer usprobe_transformDevice;
  usprobe_transformDevice = fixture.Server.Session->SendTransform(usprobe_transform_stream_name,
                                                  fixture.CreateTestTransform());

  std::string pointer_name = "pointer";
  std::string pointer_transform_stream_name = pointer_name+"_transform";

  igtlioTransformDevicePointer transformDevice;
  transformDevice = fixture.Server.Session->SendTransform(pointer_transform_stream_name,
                                                  fixture.CreateTestTransform());

    //--------------------------------------------------------------------------

  int number_of_devices = 3;
  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::NewDeviceEvent, number_of_devices))
  {
    std::cout << "ERROR: Did not get " << number_of_devices << " events" << std::endl;
    return TEST_FAILED;
  }

    //---------------------------------------------------------------------------

  std::map<std::string, int> tools;
  for(int i=0; i< fixture.Client.Logic->GetNumberOfDevices(); ++i)
  {
    igtlioDevicePointer device = fixture.Client.Logic->GetDevice(i);
    std::string tool_name = fixture.Translator.GetToolNameFromDeviceName(device->GetDeviceName());
    if(tool_name != usprobe_name && tool_name != pointer_name)
      {
        std::cout << "ERROR: tool is not what is expected: " << tool_name << std::endl;
        return TEST_FAILED;
      }
    else
        tools[tool_name] = 1;
  }

  if(tools.size() != 2)
    {
      std::cout << "ERROR: Expected 2 tools to be present, found " << tools.size() << std::endl;
      return TEST_FAILED;
    }

  std::map<std::string, int>::iterator it;
  for(it=tools.begin() ; it!=tools.end(); ++it)
    {
      std::string type = fixture.Translator.DetermineTypeBasedOnToolName(it->first);
      if(type == "unknown")
        {
          std::cout << "ERROR: Tool with name " << it->first << " is of unknown type." << std::endl;
          return TEST_FAILED;
        }
    }


  return TEST_SUCCESS;
}
