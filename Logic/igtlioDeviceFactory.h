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

namespace igtlio
{

typedef vtkSmartPointer<class DeviceCreator> vtkIGTLIODeviceCreatorPointer;
typedef vtkSmartPointer<class DeviceFactory> DeviceFactoryPointer;

class OPENIGTLINKIO_LOGIC_EXPORT DeviceFactory : public vtkObject
{
 public:
  static DeviceFactory *New();
  vtkTypeMacro(DeviceFactory,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  vtkIGTLIODeviceCreatorPointer GetCreator(std::string device_type) const;
  std::vector<std::string> GetAvailableDeviceTypes() const;

  // Create a Device object based on an input device_type
  // TODO: Should we accept prefixed message types as well?
  DevicePointer create(std::string device_type, std::string device_name) const;

protected:
  DeviceFactory();
  virtual ~DeviceFactory();

  // Register a factory for a specific Device Type.
  template<class CREATOR_TYPE>
  void registerCreator();

  std::map<std::string, vtkIGTLIODeviceCreatorPointer> Creators;
};

} // namespace igtlio

#endif /* IGTLIODEVICEFACTORY_H_ */
