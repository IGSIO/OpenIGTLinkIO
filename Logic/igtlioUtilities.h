#ifndef IGTLIOUTILITIES_H
#define IGTLIOUTILITIES_H

#include "igtlioLogicExport.h"

// std includes
#include <mutex>
#include <string>

// OpenIGTLink includes
#include "igtlMessageBase.h"

// VTK includes
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"

typedef vtkSmartPointer<class igtlioDevice> igtlioDevicePointer;

enum OPENIGTLINKIO_LOGIC_EXPORT IGTLIO_SYNCHRONIZATION_TYPE
{
  IGTLIO_BLOCKING,
  IGTLIO_ASYNCHRONOUS
};

///
/// Uniquely identify a Device by both its name and type.
/// This enables broadcast Devices (with empty name) of different types
/// to be stored in the same structures.
///
struct OPENIGTLINKIO_LOGIC_EXPORT igtlioDeviceKeyType
{
  explicit igtlioDeviceKeyType() {}
  explicit igtlioDeviceKeyType(const std::string& type_, const std::string& name_) :
    type(type_), name(name_) {}
  std::string type;
  std::string name;

public:
  std::string GetBaseTypeName() const;

  static igtlioDeviceKeyType CreateDeviceKey(igtl::MessageBase::Pointer message);
  static igtlioDeviceKeyType CreateDeviceKey(igtlioDevicePointer device);
};

bool operator==(const igtlioDeviceKeyType& lhs, const igtlioDeviceKeyType& rhs);
bool operator<(const igtlioDeviceKeyType& lhs, const igtlioDeviceKeyType& rhs);

#endif // IGTLIOUTILITIES_H
