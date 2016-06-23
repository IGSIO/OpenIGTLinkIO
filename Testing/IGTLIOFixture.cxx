#include "IGTLIOFixture.h"

#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>

LogicFixture::LogicFixture()
{
}

void LogicFixture::startClient()
{
  Logic = vtkIGTLIOLogicPointer::New();
  Connector = Logic->CreateConnector();
  Connector->SetTypeClient(Connector->GetServerHostname(), Connector->GetServerPort());
  std::cout << "Starting CLIENT connector " << Connector.GetPointer()  << std::endl;
  Connector->Start();
}

void LogicFixture::startServer()
{
  Logic = vtkIGTLIOLogicPointer::New();
  Connector = Logic->CreateConnector();
  Connector->SetTypeServer(Connector->GetServerPort());
  std::cout << "Starting SERVER connector " << Connector.GetPointer() << std::endl;
  Connector->Start();
}

vtkSmartPointer<vtkIGTLIOImageDevice> LogicFixture::CreateDummyImageDevice()
{
  vtkSmartPointer<vtkIGTLIOImageDevice> imageDevice;
  imageDevice = vtkIGTLIOImageDevice::SafeDownCast(vtkIGTLIOImageDeviceCreator::New()->Create("TestDevice_Image"));
  igtl::ImageConverter::ContentData imageContent;
  imageContent.image = vtkSmartPointer<vtkImageData>::New();
  imageContent.image->SetSpacing(1.5, 1.2, 1);
  imageContent.image->SetExtent(0, 19, 0, 49, 0, 1);
  imageContent.image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  imageContent.transform = vtkSmartPointer<vtkMatrix4x4>::New();
  imageContent.transform->Identity();

  int scalarSize = imageContent.image->GetScalarSize();
  unsigned char* ptr = reinterpret_cast<unsigned char*>(imageContent.image->GetScalarPointer());
  unsigned char color = 0;
  std::fill(ptr, ptr+scalarSize, color++);

  imageDevice->SetContent(imageContent);
  return imageDevice;
}
