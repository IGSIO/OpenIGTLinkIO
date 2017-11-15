#ifndef STRINGCONVERTER_H
#define STRINGCONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlStringMessage.h>
#include "igtlioBaseConverter.h"

namespace igtlio
{

class OPENIGTLINKIO_CONVERTER_EXPORT StringConverter : public BaseConverter
{
public:
  /**
   * Content of STRING message.
   */
  struct ContentData
  {
  unsigned int encoding;
  std::string string_msg;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "STRING"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::StringMessage::Pointer* dest);

};

} //namespace igtlio

#endif // STRINGCONVERTER_H
