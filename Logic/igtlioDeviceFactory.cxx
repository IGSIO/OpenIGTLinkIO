/*
 * vtkIGTLIODeviceFactory.cpp
 *
 *  Created on: 21. jan. 2016
 *      Author: cas
 */

#include "igtlioDeviceFactory.h"
#include <vtkObjectFactory.h>

#include "igtlioImageDevice.h"
#include "igtlioImageMetaDevice.h"
#include "igtlioLabelMetaDevice.h"
#include "igtlioPolyDataDevice.h"
#include "igtlioPointDevice.h"
#include "igtlioStatusDevice.h"
#include "igtlioStringDevice.h"
#include "igtlioTransformDevice.h"
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include "igtlioVideoDevice.h"
#endif

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioDeviceFactory);
//---------------------------------------------------------------------------
igtlioDeviceFactory::igtlioDeviceFactory()
{
  this->registerCreator<igtlioImageDeviceCreator>();
  this->registerCreator<igtlioImageMetaDeviceCreator>();
  this->registerCreator<igtlioLabelMetaDeviceCreator>();
  this->registerCreator<igtlioPolyDataDeviceCreator>();
  this->registerCreator<igtlioPointDeviceCreator>();
  this->registerCreator<igtlioStatusDeviceCreator>();
  this->registerCreator<igtlioStringDeviceCreator>();
  this->registerCreator<igtlioTransformDeviceCreator>(); 
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  this->registerCreator<igtlioVideoDeviceCreator>();
#endif
}

//---------------------------------------------------------------------------
igtlioDeviceFactory::~igtlioDeviceFactory()
{
}

//---------------------------------------------------------------------------
void igtlioDeviceFactory::PrintSelf(ostream &os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
igtlioDeviceCreatorPointer igtlioDeviceFactory::GetCreator(std::string device_type) const
{
  std::map<std::string, igtlioDeviceCreatorPointer>::const_iterator iter = Creators.find(device_type);
  if (iter==Creators.end())
    {
    return igtlioDeviceCreatorPointer();
    }
  return iter->second;
}

std::vector<std::string> igtlioDeviceFactory::GetAvailableDeviceTypes() const
{
  std::vector<std::string> retval;
  for (std::map<std::string, igtlioDeviceCreatorPointer>::const_iterator iter=Creators.begin();
       iter!=Creators.end();
       ++iter)
    {
    retval.push_back(iter->first);
    }
  return retval;
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioDeviceFactory::create(std::string device_type, std::string device_name) const
{
  igtlioDeviceCreatorPointer creator = this->GetCreator(device_type);
  if (!creator)
  {
    return igtlioDevicePointer();
  }
  return creator->Create(device_name);
}

//---------------------------------------------------------------------------
template<class CREATOR_TYPE>
void igtlioDeviceFactory::registerCreator()
{
  igtlioDeviceCreatorPointer creator = vtkSmartPointer<CREATOR_TYPE>::New();
  Creators[creator->GetDeviceType()] = creator;
}
