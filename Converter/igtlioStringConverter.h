#ifndef STRINGCONVERTER_H
#define STRINGCONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlStringMessage.h>
#include "igtlioBaseConverter.h"

class OPENIGTLINKIO_CONVERTER_EXPORT igtlioStringConverter : public igtlioBaseConverter
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

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::StringMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif // STRINGCONVERTER_H
