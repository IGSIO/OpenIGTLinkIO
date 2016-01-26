/*
 * vtkIGTLIODeviceFactory.h
 *
 *  Created on: 21. jan. 2016
 *      Author: cas
 */

#ifndef vtkIGTLIODEVICEFACTORY_H_
#define vtkIGTLIODEVICEFACTORY_H_

#include <map>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// IGTLIO includes
#include "igtlioLogicExport.h"
#include "vtkIGTLIODevice.h"

typedef vtkSmartPointer<class vtkIGTLIODevice> vtkIGTLIODevicePointer;
typedef vtkSmartPointer<class vtkIGTLIODeviceCreator> vtkIGTLIODeviceCreatorPointer;
typedef vtkSmartPointer<class vtkIGTLIODeviceFactory> vtkIGTLIODeviceFactoryPointer;

class OPENIGTLINKIO_LOGIC_EXPORT vtkIGTLIODeviceFactory : public vtkObject
{
 public:
  static vtkIGTLIODeviceFactory *New();
  vtkTypeMacro(vtkIGTLIODeviceFactory,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  vtkIGTLIODeviceCreatorPointer GetCreator(std::string device_type) const;

  // Create a Device object based on an input device_type
  // TODO: Should we accept prefixed message types as well?
  vtkIGTLIODevicePointer create(std::string device_type, std::string device_name) const;

  // Register a factory for a specific Device.
  // CHANGED: Registration is done internally.
  //void RegisterFactory(vtkSmartPointer<vtkIGTLIODevice::Creator> factory);

protected:
  vtkIGTLIODeviceFactory();
  virtual ~vtkIGTLIODeviceFactory();

  std::map<std::string, vtkIGTLIODeviceCreatorPointer> Creators;
};

#endif /* vtkIGTLIODEVICEFACTORY_H_ */
