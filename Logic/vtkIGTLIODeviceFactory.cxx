/*
 * vtkIGTLIODeviceFactory.cpp
 *
 *  Created on: 21. jan. 2016
 *      Author: cas
 */

#include "vtkIGTLIODeviceFactory.h"
#include <vtkObjectFactory.h>

#include "vtkIGTLIOImageDevice.h"
#include "vtkIGTLIOStatusDevice.h"
#include "vtkIGTLIOCommandDevice.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIODeviceFactory);
//---------------------------------------------------------------------------
vtkIGTLIODeviceFactory::vtkIGTLIODeviceFactory()
{
  this->registerCreator<vtkIGTLIOImageDeviceCreator>();
  this->registerCreator<vtkIGTLIOStatusDeviceCreator>();
  this->registerCreator<vtkIGTLIOCommandDeviceCreator>();
}

//---------------------------------------------------------------------------
vtkIGTLIODeviceFactory::~vtkIGTLIODeviceFactory()
{
}

//---------------------------------------------------------------------------
void vtkIGTLIODeviceFactory::PrintSelf(ostream &os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
vtkIGTLIODeviceCreatorPointer vtkIGTLIODeviceFactory::GetCreator(std::string device_type) const
{
  std::map<std::string, vtkIGTLIODeviceCreatorPointer>::const_iterator iter = Creators.find(device_type);
  if (iter==Creators.end())
    {
    return vtkIGTLIODeviceCreatorPointer();
    }
  return iter->second;
}

//---------------------------------------------------------------------------
vtkIGTLIODevicePointer vtkIGTLIODeviceFactory::create(std::string device_type, std::string device_name) const
{
  vtkIGTLIODeviceCreatorPointer creator = this->GetCreator(device_type);
  if (!creator)
    return vtkIGTLIODevicePointer();
  return creator->Create(device_name);
}

//---------------------------------------------------------------------------
template<class CREATOR_TYPE>
void vtkIGTLIODeviceFactory::registerCreator()
{
  vtkIGTLIODeviceCreatorPointer creator = vtkSmartPointer<CREATOR_TYPE>::New();
  Creators[creator->GetDeviceType()] = creator;
}

