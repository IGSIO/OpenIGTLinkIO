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
#include "igtlioImageDevice.h"
#include "igtlioSession.h"
#include "igtlioTestUtilities.h"

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

  igtlioImageDevicePointer imageDevice;
  imageDevice = fixture.Server.Session->SendImage("TestDevice_Image",
                                                  fixture.CreateTestImage(),
                                                  fixture.CreateTestTransform());
  std::cout << "*** Sent message from Server to Client" << std::endl;

  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::NewDeviceEvent))
  {
    return TEST_FAILED;
  }

  //---------------------------------------------------------------------------
  // The device modified event should be invoke when the setContent was called in the device
  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::DeviceModifiedEvent))
  {
    return TEST_FAILED;
  }

  if (fixture.Client.Logic->GetNumberOfDevices() == 0)
  {
    std::cout << "FAILURE: No devices received." << std::endl;
    return TEST_FAILED;
  }

  igtlioImageDevicePointer receivedDevice;
  receivedDevice = igtlioImageDevice::SafeDownCast(fixture.Client.Logic->GetDevice(0));
  if (!receivedDevice)
  {
    std::cout << "FAILURE: Non-image device received." << std::endl;
    return TEST_FAILED;
  }

  std::cout << "*** Client received IMAGE device." << std::endl;
  //---------------------------------------------------------------------------

  if (!compare(imageDevice, receivedDevice))
  {
    std::cout << "FAILURE: Image differs from the one sent from server." << std::endl;
    return TEST_FAILED;
  }
  return TEST_SUCCESS;
}
