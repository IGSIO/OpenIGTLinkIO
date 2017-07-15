/*
 * vtkIGTLIODeviceFactory.cpp
 *
 *  Created on: 21. jan. 2016
 *      Author: cas
 */

#include "igtlioDeviceFactory.h"
#include <vtkObjectFactory.h>

#include "igtlioImageDevice.h"
#include "igtlioStatusDevice.h"
#include "igtlioCommandDevice.h"
#include "igtlioTransformDevice.h"
//#include "igtlioVideoDevice.h"

namespace igtlio
{

//---------------------------------------------------------------------------
vtkStandardNewMacro(DeviceFactory);
//---------------------------------------------------------------------------
DeviceFactory::DeviceFactory()
{
  this->registerCreator<ImageDeviceCreator>();
  this->registerCreator<StatusDeviceCreator>();
  this->registerCreator<CommandDeviceCreator>();
  this->registerCreator<igtlio::TransformDeviceCreator>();
  //this->registerCreator<igtlio::VideoDeviceCreator>();
}

//---------------------------------------------------------------------------
DeviceFactory::~DeviceFactory()
{
}

//---------------------------------------------------------------------------
void DeviceFactory::PrintSelf(ostream &os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
vtkIGTLIODeviceCreatorPointer DeviceFactory::GetCreator(std::string device_type) const
{
  std::map<std::string, vtkIGTLIODeviceCreatorPointer>::const_iterator iter = Creators.find(device_type);
  if (iter==Creators.end())
    {
    return vtkIGTLIODeviceCreatorPointer();
    }
  return iter->second;
}

std::vector<std::string> DeviceFactory::GetAvailableDeviceTypes() const
{
  std::vector<std::string> retval;
  for (std::map<std::string, vtkIGTLIODeviceCreatorPointer>::const_iterator iter=Creators.begin();
       iter!=Creators.end();
       ++iter)
    {
    retval.push_back(iter->first);
    }
  return retval;
}

//---------------------------------------------------------------------------
DevicePointer DeviceFactory::create(std::string device_type, std::string device_name) const
{
  vtkIGTLIODeviceCreatorPointer creator = this->GetCreator(device_type);
  if (!creator)
    return DevicePointer();
  return creator->Create(device_name);
}

//---------------------------------------------------------------------------
template<class CREATOR_TYPE>
void DeviceFactory::registerCreator()
{
  vtkIGTLIODeviceCreatorPointer creator = vtkSmartPointer<CREATOR_TYPE>::New();
  Creators[creator->GetDeviceType()] = creator;
}

} // namespace igtlio

