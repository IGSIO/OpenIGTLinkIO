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

namespace igtlio
{

/** Conversion between igtl::StatusMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT CommandConverter : public BaseConverter
{
public:

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

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "COMMAND"; }
  static const char* GetIGTLResponseName() { return "RTS_COMMAND"; }


  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  static int fromIGTLResponse(igtl::MessageBase::Pointer source, HeaderData *header, ContentData *dest, bool checkCRC);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::CommandMessage::Pointer* dest);

  static std::vector<std::string> GetAvailableCommandNames();
};

} // namespace igtlio

#endif // IGTLCOMMANDCONVERTER_H
