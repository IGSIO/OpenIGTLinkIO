#ifndef IGTLSTATUSCONVERTER_H
#define IGTLSTATUSCONVERTER_H

#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlStatusMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include "igtlBaseConverter.h"

namespace igtl
{

/** Conversion between igtl::StatusMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT StatusConverter : public BaseConverter
{
public:
 /** Standard class typedefs. */
 typedef StatusConverter        Self;
 typedef LightObject              Superclass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Method for creation through the object factory. */
 igtlNewMacro(Self);

 /** Run-time type information (and related methods). */
 igtlTypeMacro(StatusConverter, BaseConverter);

  /**
   * Content of STATUS message.
   */
  struct ContentData
  {
  int code;
  int subcode;
  std::string errorname;
  std::string statusstring;
  };

  virtual void PrintSelf(std::ostream& os) const;

  virtual const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "STATUS"; }

  int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  int toIGTL(const HeaderData& header, const ContentData& source, igtl::StatusMessage::Pointer* dest);

protected:
  StatusConverter();
  ~StatusConverter();
};

}

#endif // IGTLSTATUSCONVERTER_H
