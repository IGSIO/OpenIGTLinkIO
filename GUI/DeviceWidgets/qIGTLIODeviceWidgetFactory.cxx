#include "qIGTLIODeviceWidgetFactory.h"
#include <vtkObjectFactory.h>
#include "qIGTLIODeviceWidget.h"
#include "qIGTLIOGenericDeviceWidget.h"

#include "vtkIGTLIOImageDevice.h"
#include "qIGTLIOStatusDeviceWidget.h"
#include "qIGTLIOCommandDeviceWidget.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIODeviceWidgetFactory);
//---------------------------------------------------------------------------
vtkIGTLIODeviceWidgetFactory::vtkIGTLIODeviceWidgetFactory()
{
  this->registerCreator<vtkIGTLIOStatusDeviceWidgetCreator>();
  this->registerCreator<vtkIGTLIOCommandDeviceWidgetCreator>();
}

//---------------------------------------------------------------------------
vtkIGTLIODeviceWidgetFactory::~vtkIGTLIODeviceWidgetFactory()
{
}

//---------------------------------------------------------------------------
void vtkIGTLIODeviceWidgetFactory::PrintSelf(ostream &os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
vtkIGTLIODeviceWidgetCreatorPointer vtkIGTLIODeviceWidgetFactory::GetCreator(std::string device_type) const
{
  std::map<std::string, vtkIGTLIODeviceWidgetCreatorPointer>::const_iterator iter = Creators.find(device_type);
  if (iter==Creators.end())
    {
    return vtkIGTLIODeviceWidgetCreatorPointer();
    }
  return iter->second;
}

//---------------------------------------------------------------------------
qIGTLIODeviceWidget* vtkIGTLIODeviceWidgetFactory::create(std::string device_type) const
{
  vtkIGTLIODeviceWidgetCreatorPointer creator = this->GetCreator(device_type);
  if (!creator)
    return new qIGTLIOGenericDeviceWidget();
  return creator->Create();
}

//---------------------------------------------------------------------------
template<class CREATOR_TYPE>
void vtkIGTLIODeviceWidgetFactory::registerCreator()
{
  vtkIGTLIODeviceWidgetCreatorPointer creator = vtkSmartPointer<CREATOR_TYPE>::New();
  Creators[creator->GetDeviceType()] = creator;
}

