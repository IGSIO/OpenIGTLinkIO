#include "igtlioTestUtilities.h"

#include <vtkImageDifference.h>

namespace igtlio
{

vtkSmartPointer<vtkImageData> CreateTestImage()
{
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image->SetSpacing(1.5, 1.2, 1);
  image->SetExtent(0, 19, 0, 49, 0, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  int scalarSize = image->GetScalarSize();
  unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
  unsigned char color = 0;
  std::fill(ptr, ptr+scalarSize, color++);

  return image;
}

vtkSmartPointer<vtkMatrix4x4> CreateTestTransform()
{
  vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->Identity();
  return transform;
}

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

std::string boolToString(bool b)
{
  std::string ret = b ? "true" : "false";
  return ret;
}

bool contains(std::vector<int> input, int value, int count)
{
  int found_times = 0;
  for(int i=0; i<input.size(); ++i)
    {
      std::cout << "i: " << input[i] << std::endl;
      if(input[i] == value)
        found_times+=1;
    }
  return (found_times >= count) ? true : false;
}

}

