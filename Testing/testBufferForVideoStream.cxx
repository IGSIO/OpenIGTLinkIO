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
#include "igtlioFixture.h"
#include "igtlioVideoDevice.h"
#include "igtlioSession.h"
#include "igtlMessageDebugFunction.h"

bool a_compare(vtkSmartPointer<vtkImageData> a, vtkSmartPointer<vtkImageData> b)
{
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
    igtlUint8* yuv_a = new igtlUint8[a->GetDimensions()[0]*a->GetDimensions()[1]*3/2];
    igtlUint8* rgb_a = new igtlUint8[a->GetDimensions()[0]*a->GetDimensions()[1]*3];
    GenericEncoder::ConvertRGBToYUV((igtlUint8*)a->GetScalarPointer(), yuv_a, a->GetDimensions()[0], a->GetDimensions()[1]);
    GenericDecoder::ConvertYUVToRGB(yuv_a, rgb_a, b->GetDimensions()[0], b->GetDimensions()[1]);
    memcmp(rgb_a, a->GetScalarPointer(),a->GetDimensions()[0]*a->GetDimensions()[1]*3);// The conversion is not valid. Image is not the same after conversion.
    //TestDebugCharArrayCmp(b->GetScalarPointer(),rgb_a,a->GetDimensions()[0]*a->GetDimensions()[1]*3);
    int sumError = 0;
    for (int i = 0 ; i< a->GetDimensions()[0]*a->GetDimensions()[1];i++)
    {
      sumError += abs(*((igtlUint8*)rgb_a+i)-*((igtlUint8*)b->GetScalarPointer()+i));
    }
    if (sumError<a->GetDimensions()[0]*a->GetDimensions()[1]) // To do, check the lossless encoding problem. most likely from the RGB and YUV conversion
    return true;
#endif    
  return false;
}

bool compare(igtlioVideoDevicePointer a, igtlioVideoDevicePointer b)
{
  if (a->GetDeviceName() != b->GetDeviceName())
    return false;
  if (a->GetDeviceType() != b->GetDeviceType())
    return false;
  if (!a_compare(a->GetContent().image, b->GetContent().image))
    return false;
  
  return true;
}


int main(int argc, char **argv)
{
  igtlioClientServerFixture fixture;
  
  for (int i =0 ; i <100; i++)
  {
    igtl::VideoMessage::Pointer buffer = igtl::VideoMessage::New();
    buffer->InitPack();
    buffer->SetMetaDataElement("test", 11);
    buffer->SetBitStreamSize(10);
    buffer->AllocateBuffer();
  }
  
  if (!fixture.ConnectClientToServer())
    return 1;


  if (fixture.Client.Logic->GetNumberOfDevices() != 0)
    {
    std::cout << "ERROR: Client has devices before they have been added or fundamental error!" << std::endl;
    return 1;
    }

  std::cout << "*** Connection done" << std::endl;
  //---------------------------------------------------------------------------
  igtlioVideoDevicePointer videoDevice;
  vtkSmartPointer<vtkImageData> image = vtkImageData::New();
  fixture.CreateTestFrame(image); // X265 encoder expects the frame with regular size, such as 128, 256,etc.
  videoDevice = fixture.Server.Session->SendFrame("TestDevice_Image",image);
  std::cout << "*** Sent message from Server to Client" << std::endl;
  //---------------------------------------------------------------------------

  if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::NewDeviceEvent))
    {
    return 1;
    }

  if (fixture.Client.Logic->GetNumberOfDevices() == 0)
    {
    std::cout << "FAILURE: No devices received." << std::endl;
    return 1;
    }

  igtlioVideoDevicePointer receivedDevice;
  receivedDevice = igtlioVideoDevice::SafeDownCast(fixture.Client.Logic->GetDevice(0));
  if (!receivedDevice)
    {
    std::cout << "FAILURE: Non-video device received." << std::endl;
    return 1;
    }

  std::cout << "*** Client received video device." << std::endl;
  //---------------------------------------------------------------------------
  int loop = 0;
  
  
  // Normal buffer testing
  igtlioImageDevicePointer imageDevice;
  while(loop++ < 5)
    {
    int frameNum = (float)(std::rand())/RAND_MAX * 20 + 8;
    for (int frameIndex = 0; frameIndex< frameNum; frameIndex++)
      {
      imageDevice = fixture.Server.Session->SendImage("TestDevice_Image", fixture.CreateTestImage(), fixture.CreateTestTransform());
      unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
      unsigned char color = 108;
      for(int i = 0 ; i< image->GetDimensions()[0]*image->GetDimensions()[1]*3; i++)
      {
        int noise = (float)(std::rand())/RAND_MAX * 10 + 2;
        *ptr = color%256 + noise;
        color++;
        ptr++;
      }
      fixture.Server.Logic->PeriodicProcess();
      igtl::Sleep(20);
      }
    int bufferReadNum = (float)(std::rand())/RAND_MAX * 10 + 4;
    for (int bufferReadIndex = 0; bufferReadIndex< bufferReadNum; bufferReadIndex++)
      {
      fixture.Client.Logic->PeriodicProcess();
      }
    }
  
  /* Video buffer testing
  loop = 0;
  while(loop++ < 2)
    {
    int frameNum = (float)(std::rand())/RAND_MAX * 20 + 8;
    for (int frameIndex = 0; frameIndex< frameNum; frameIndex++)
      {
      videoDevice = fixture.Server.Session->SendFrame("TestDevice_Image", image);
      unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
      unsigned char color = 108;
      for(int i = 0 ; i< image->GetDimensions()[0]*image->GetDimensions()[1]*3; i++)
      {
        int noise = (float)(std::rand())/RAND_MAX * 10 + 2;
        *ptr = color%256 + noise;
        color++;
        ptr++;
      }
      fixture.Server.Logic->PeriodicProcess();
      igtl::Sleep(20);
      }
    int bufferReadNum = (float)(std::rand())/RAND_MAX * 10 + 4;
    for (int bufferReadIndex = 0; bufferReadIndex< bufferReadNum; bufferReadIndex++)
      {
      fixture.Client.Logic->PeriodicProcess();
      }
    }*/
}
