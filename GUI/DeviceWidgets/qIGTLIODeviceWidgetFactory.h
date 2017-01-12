#ifndef QIGTLIODEVICEWIDGETFACTORY_H
#define QIGTLIODEVICEWIDGETFACTORY_H

#include <map>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// IGTLIO includes
#include "igtlioGUIExport.h"
#include "igtlioDevice.h"

class qIGTLIODeviceWidget;
typedef vtkSmartPointer<class vtkIGTLIODeviceWidgetCreator> vtkIGTLIODeviceWidgetCreatorPointer;
typedef vtkSmartPointer<class vtkIGTLIODeviceWidgetFactory> vtkIGTLIODeviceWidgetFactoryPointer;

class OPENIGTLINKIO_GUI_EXPORT vtkIGTLIODeviceWidgetFactory : public vtkObject
{
 public:
  static vtkIGTLIODeviceWidgetFactory *New();
  vtkTypeMacro(vtkIGTLIODeviceWidgetFactory,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  vtkIGTLIODeviceWidgetCreatorPointer GetCreator(std::string device_type) const;

  // Create a Device object based on an input device_type
  qIGTLIODeviceWidget *create(std::string device_type) const;

protected:
  vtkIGTLIODeviceWidgetFactory();
  virtual ~vtkIGTLIODeviceWidgetFactory();

  // Register a factory for a specific Device Type.
  template<class CREATOR_TYPE>
  void registerCreator();

  std::map<std::string, vtkIGTLIODeviceWidgetCreatorPointer> Creators;
};


#endif // QIGTLIODEVICEWIDGETFACTORY_H
