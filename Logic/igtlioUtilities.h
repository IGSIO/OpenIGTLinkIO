#ifndef IGTLIOUTILITIES_H
#define IGTLIOUTILITIES_H

#include "igtlioLogicExport.h"
#include <string>
#include "igtlMessageBase.h"
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

///
/// A class for automatically unlocking objects
/// This class is used for locking a objects (buffers, mutexes, etc.)
/// and releasing the lock automatically when the guard object is deleted
/// (typically by getting out of scope).
///
/// Example:
/// \code
/// igtlioLockGuard<vtkMutexLock> updateMutexGuardedLock(this->UpdateMutex);
/// \endcode
///
template <typename T>
class igtlioLockGuard
{
public:
  igtlioLockGuard(T* lockableObject)
  {
    m_LockableObject = lockableObject;
    m_LockableObject->Lock();
  }
  ~igtlioLockGuard()
  {
    m_LockableObject->Unlock();
    m_LockableObject = NULL;
  }
private:
  igtlioLockGuard(igtlioLockGuard&);
  void operator=(igtlioLockGuard&);

  T* m_LockableObject;
};

#endif // IGTLIOUTILITIES_H
