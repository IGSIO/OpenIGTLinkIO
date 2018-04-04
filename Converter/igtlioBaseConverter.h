#ifndef IGTLIOBASECONVERTER_H
#define IGTLIOBASECONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlMessageBase.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>


//---------------------------------------------------------------------------

// TODO: Move this into igtlSmartPointer.h
/// implements smart pointer conversion for igtl::SmartPointer,
/// similar to std::dynamic_pointer_cast
template <class T, class U>
igtl::SmartPointer<T> dynamic_pointer_cast(const igtl::SmartPointer<U>& sp) //noexcept
{
 T* ptr = dynamic_cast<T*>(sp.GetPointer());
 return igtl::SmartPointer<T>(ptr);
}
//---------------------------------------------------------------------------

namespace igtlio
{

/** Conversion between igtl::ImageMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT BaseConverter
{
public:
  /**
   * This structure contains data common to all igtl messages,
   * TODO: add header xml-data here.
   */
  struct HeaderData
  {
  std::string deviceName;
  double timestamp;
  };

  static int IGTLtoHeader(igtl::MessageBase::Pointer source, HeaderData* header, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int HeadertoIGTL(const HeaderData& header, igtl::MessageBase::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

  static int IGTLToTimestamp(igtl::MessageBase::Pointer msg, HeaderData *dest);
};

} // namespace igtlio

#endif // IGTLIOBASECONVERTER_H
