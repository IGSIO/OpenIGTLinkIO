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
	enum EQUIPMENT_TYPE {
		UNKNOWN = 0,
		VIDEO, //images
		US_PROBE, //images + strings/params
		CONTROLLER, //strings/params (us_scanner, hardware controlling something, ekg...)
		TRACKED_TOOL, //transforms
		TRACKED_VIDEO, //transforms + images
		TRACKED_US_PROBE //images + transforms + strings/params
	};

public:
  /**
   * This structure contains data common to all igtl messages,
   * TODO: add header xml-data here.
   */
  struct HeaderData
  {
  std::string deviceName;
  double timestamp;
  EQUIPMENT_TYPE equipmentType;
  std::string equipmentId;
  };

  static int IGTLtoHeader(igtl::MessageBase::Pointer source, HeaderData* header);
  static int HeadertoIGTL(const HeaderData& header, igtl::MessageBase::Pointer* dest);

  static int IGTLToTimestamp(igtl::MessageBase::Pointer msg, HeaderData *dest);

};

} // namespace igtlio

#endif // IGTLIOBASECONVERTER_H
