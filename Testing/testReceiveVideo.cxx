#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include "vtkImageData.h"
#include "igtlioVideoConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "igtlioVideoConverter.h"
#include <vtkImageDifference.h>
#include "IGTLIOFixture.h"
#include "igtlioVideoDevice.h"
#include "igtlioSession.h"


bool compare(vtkSmartPointer<vtkImageData> a, vtkSmartPointer<vtkImageData> b)
{
  vtkSmartPointer<vtkImageDifference> differenceFilter = vtkSmartPointer<vtkImageDifference>::New();
  differenceFilter->SetInputData(a);
  differenceFilter->SetImageData(b);
  differenceFilter->Update();
  double imageError = differenceFilter->GetError();
  if (fabs(imageError) > 2) // for lossy compression transmission
    return false;
  return true;
}

bool compare(igtlio::VideoDevicePointer a, igtlio::VideoDevicePointer b)
{
  if (a->GetDeviceName() != b->GetDeviceName())
    return false;
  if (fabs(a->GetTimestamp()-b->GetTimestamp()) < 1E-3)
    return false;
  if (a->GetDeviceType() != b->GetDeviceType())
    return false;
  if (!compare(a->GetContent().image, b->GetContent().image))
    return false;
  
  return true;
}


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

  igtlio::VideoDevicePointer videoDevice;
  videoDevice = fixture.Server.Session->SendFrame("TestDevice_Image",
                                                  fixture.CreateTestFrame());
  std::cout << "*** Sent message from Server to Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlio::Logic::NewDeviceEvent))
  {
    return 1;
  }

  if (fixture.Client.Logic->GetNumberOfDevices() == 0)
  {
    std::cout << "FAILURE: No devices received." << std::endl;
    return 1;
  }

  igtlio::VideoDevicePointer receivedDevice;
  receivedDevice = igtlio::VideoDevice::SafeDownCast(fixture.Client.Logic->GetDevice(0));
  if (!receivedDevice)
  {
    std::cout << "FAILURE: Non-video device received." << std::endl;
    return 1;
  }

  std::cout << "*** Client received video device." << std::endl;
  //---------------------------------------------------------------------------

  if (!compare(videoDevice, receivedDevice))
  {
    std::cout << "FAILURE: frame differs from the one sent from server." << std::endl;
    return 1;
  }
}
