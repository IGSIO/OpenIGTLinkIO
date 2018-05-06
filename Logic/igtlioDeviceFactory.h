/*
 * vtkIGTLIODeviceFactory.h
 *
 *  Created on: 21. jan. 2016
 *      Author: cas
 */

#ifndef IGTLIODEVICEFACTORY_H_
#define IGTLIODEVICEFACTORY_H_

#include <map>
#include <string>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// IGTLIO includes
#include "igtlioLogicExport.h"
#include "igtlioDevice.h"

typedef vtkSmartPointer<class igtlioDeviceCreator> igtlioDeviceCreatorPointer;
typedef vtkSmartPointer<class igtlioDeviceFactory> igtlioDeviceFactoryPointer;

class OPENIGTLINKIO_LOGIC_EXPORT igtlioDeviceFactory : public vtkObject
{
 public:
  static igtlioDeviceFactory *New();
  vtkTypeMacro(igtlioDeviceFactory,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  igtlioDeviceCreatorPointer GetCreator(std::string device_type) const;
  std::vector<std::string> GetAvailableDeviceTypes() const;

  // Create a Device object based on an input device_type
  // TODO: Should we accept prefixed message types as well?
  igtlioDevicePointer create(std::string device_type, std::string device_name) const;

protected:
  igtlioDeviceFactory();
  virtual ~igtlioDeviceFactory();

  // Register a factory for a specific Device Type.
  template<class CREATOR_TYPE>
  void registerCreator();

  std::map<std::string, igtlioDeviceCreatorPointer> Creators;
};

#endif /* IGTLIODEVICEFACTORY_H_ */
