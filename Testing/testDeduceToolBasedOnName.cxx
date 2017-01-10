#include "IGTLIOFixture.h"
#include "vtkIGTLIOSession.h"
#include "vtkIGTLIOTransformDevice.h"

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

  std::string usprobe_name = "usprobe";
  std::string usprobe_image_stream_name = usprobe_name+"_image";
  std::string usprobe_transform_stream_name = usprobe_name+"_transform";

  vtkSmartPointer<vtkIGTLIOImageDevice> imageDevice;
  imageDevice = fixture.Server.Session->SendImage(usprobe_image_stream_name,
                                                  fixture.CreateTestImage(),
                                                  fixture.CreateTestTransform());

  vtkSmartPointer<vtkIGTLIOTransformDevice> usprobe_transformDevice;
  usprobe_transformDevice = fixture.Server.Session->SendTransform(usprobe_transform_stream_name,
                                                  fixture.CreateTestTransform());

  std::string pointer_name = "pointer";
  std::string pointer_transform_stream_name = pointer_name+"_transform";

  vtkSmartPointer<vtkIGTLIOTransformDevice> transformDevice;
  transformDevice = fixture.Server.Session->SendTransform(pointer_transform_stream_name,
                                                  fixture.CreateTestTransform());

    //--------------------------------------------------------------------------

  int number_of_devices = 3;
  for(int i=0; i<number_of_devices; ++i)
  {
      if (!fixture.LoopUntilEventDetected(&fixture.Server, vtkIGTLIOLogic::NewDeviceEvent))
      {
        std::cout << "ERROR: Did not get event number:" << i+1 << std::endl;
        return 1;
      }
  }
    //---------------------------------------------------------------------------

  std::map<std::string, int> tools;
  for(int i=0; i< fixture.Client.Logic->GetNumberOfDevices(); ++i)
  {
    vtkIGTLIODevicePointer device = fixture.Client.Logic->GetDevice(i);
    std::string tool_name = fixture.Translator.GetToolFromName(device->GetDeviceName());
    if(tool_name != usprobe_name && tool_name != pointer_name)
        return 1;
    else
        tools[tool_name] = 1;
  }

  if(tools.size() != 2)
      return 1;


  return 0;
}
