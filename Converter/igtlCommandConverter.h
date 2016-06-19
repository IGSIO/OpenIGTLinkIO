#ifndef IGTLCOMMANDCONVERTER_H
#define IGTLCOMMANDCONVERTER_H

#include <vector>

#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlCommandMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include "igtlBaseConverter.h"

namespace igtl
{

/** Conversion between igtl::StatusMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT CommandConverter : public BaseConverter
{
public:
 /** Standard class typedefs. */
 typedef CommandConverter        Self;
 typedef LightObject              Superclass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Method for creation through the object factory. */
 igtlNewMacro(Self);

 /** Run-time type information (and related methods). */
 igtlTypeMacro(CommandConverter, BaseConverter);

  /**
   * Content of COMMAND message.
   */
  struct ContentData
  {
    ContentData() : id(0) {}
    int id;
    std::string name;
    std::string content;
  };

  virtual void PrintSelf(std::ostream& os) const;

  virtual const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "COMMAND"; }
  static const char* GetIGTLResponseName() { return "RTS_COMMAND"; }


  int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  int toIGTL(const HeaderData& header, const ContentData& source, igtl::CommandMessage::Pointer* dest);

  std::vector<std::string> GetAvailableCommandNames() const;

protected:
  CommandConverter();
  ~CommandConverter();
};

}

#endif // IGTLCOMMANDCONVERTER_H
