#ifndef IGTLIOSTATUSCONVERTER_H
#define IGTLIOSTATUSCONVERTER_H

#include "igtlioConverterExport.h"

#include <igtlStatusMessage.h>
#include "igtlioBaseConverter.h"

class OPENIGTLINKIO_CONVERTER_EXPORT igtlioStatusConverter : public igtlioBaseConverter
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

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::StatusMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif // IGTLIOSTATUSCONVERTER_H
