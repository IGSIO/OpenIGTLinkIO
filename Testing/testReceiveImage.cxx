#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "igtlioImageDevice.h"
#include "vtkImageData.h"
#include "igtlioImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "igtlioImageConverter.h"
#include <vtkImageDifference.h>
#include "IGTLIOFixture.h"
#include "igtlioImageDevice.h"
#include "vtkIGTLIOSession.h"

bool compare(vtkSmartPointer<vtkMatrix4x4> a, vtkSmartPointer<vtkMatrix4x4> b)
{
  for (int x=0; x<4; ++x)
  {
    for (int y=0; y<4; ++y)
    {
      if (fabs(b->Element[x][y] - a->Element[x][y]) > 1E-3)
        return false;
    }
  }
  return true;
}


bool compare(vtkSmartPointer<vtkImageData> a, vtkSmartPointer<vtkImageData> b)
{
  vtkSmartPointer<vtkImageDifference> differenceFilter = vtkSmartPointer<vtkImageDifference>::New();
  differenceFilter->SetInputData(a);
  differenceFilter->SetImageData(b);
  differenceFilter->Update();
  double imageError = differenceFilter->GetError();
  if (fabs(imageError) > 1E-3)
    return false;
  return true;
}

bool compare(igtlio::ImageDevicePointer a, igtlio::ImageDevicePointer b)
{
  if (a->GetDeviceName() != b->GetDeviceName())
    return false;
  if (fabs(a->GetTimestamp()-b->GetTimestamp()) < 1E-3)
    return false;
  if (a->GetDeviceType() != b->GetDeviceType())
    return false;
  if (!compare(a->GetContent().image, b->GetContent().image))
    return false;
  if (!compare(a->GetContent().transform, b->GetContent().transform))
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

  igtlio::ImageDevicePointer imageDevice;
  imageDevice = fixture.Server.Session->SendImage("TestDevice_Image",
                                                  fixture.CreateTestImage(),
                                                  fixture.CreateTestTransform());
  std::cout << "*** Sent message from Server to Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlio::vtkIGTLIOLogic::NewDeviceEvent))
  {
    return 1;
  }

  if (fixture.Client.Logic->GetNumberOfDevices() == 0)
  {
    std::cout << "FAILURE: No devices received." << std::endl;
    return 1;
  }

  igtlio::ImageDevicePointer receivedDevice;
  receivedDevice = igtlio::ImageDevice::SafeDownCast(fixture.Client.Logic->GetDevice(0));
  if (!receivedDevice)
  {
    std::cout << "FAILURE: Non-image device received." << std::endl;
    return 1;
  }

  std::cout << "*** Client received IMAGE device." << std::endl;
  //---------------------------------------------------------------------------

  if (!compare(imageDevice, receivedDevice))
  {
    std::cout << "FAILURE: Image differs from the one sent from server." << std::endl;
    return 1;
  }
}
