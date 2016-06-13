#ifndef IGTLBASECONVERTER_H
#define IGTLBASECONVERTER_H

#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlMessageBase.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include "igtlLightObject.h"

/// implements smart pointer conversion for igtl::SmartPointer,
/// similar to std::dynamic_pointer_cast
/// TODO: Move this into igtlSmartPointer.h
template <class T, class U>
igtl::SmartPointer<T> dynamic_pointer_cast(const igtl::SmartPointer<U>& sp) //noexcept
{
 T* ptr = dynamic_cast<T*>(sp.GetPointer());
 return igtl::SmartPointer<T>(ptr);
}
//---------------------------------------------------------------------------

namespace igtl
{


/** Conversion between igtl::ImageMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT BaseConverter : public LightObject
{
public:
 /** Standard class typedefs. */
 typedef BaseConverter            Self;
 typedef LightObject              Superclass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Method for creation through the object factory. */
 igtlNewMacro(Self);

  /**
   * This structure contains data common to all igtl messages,
   * TODO: add header xml-data here.
   */
  struct HeaderData
  {
  std::string deviceName;
  double timestamp;
  };

  int IGTLtoHeader(igtl::MessageBase::Pointer source, HeaderData* header);
  int HeadertoIGTL(const HeaderData& header, igtl::MessageBase::Pointer* dest);

  int IGTLToTimestamp(igtl::MessageBase::Pointer msg, HeaderData *dest);

protected:
  BaseConverter() {}
  ~BaseConverter() {}

};

}

#endif // IGTLBASECONVERTER_H
