#ifndef IGTLIOSTATUSCONVERTER_H
#define IGTLIOSTATUSCONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlStatusMessage.h>
#include "igtlioBaseConverter.h"

namespace igtlio
{

class OPENIGTLINKIO_CONVERTER_EXPORT StatusConverter : public BaseConverter
{
public:
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

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "STATUS"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::StatusMessage::Pointer* dest);

};

} // namespace igtlio

#endif // IGTLIOSTATUSCONVERTER_H
