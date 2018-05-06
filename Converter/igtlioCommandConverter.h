#ifndef IGTLIOCOMMANDCONVERTER_H
#define IGTLIOCOMMANDCONVERTER_H

#include <vector>
#include <igtlCommandMessage.h>

#include "igtlioConverterExport.h"
#include "igtlioBaseConverter.h"

/** Conversion between igtl::StatusMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT igtlioCommandConverter : public igtlioBaseConverter
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
    bool status;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "COMMAND"; }
  static const char* GetIGTLResponseName() { return "RTS_COMMAND"; }


  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int fromIGTLResponse(igtl::MessageBase::Pointer source, HeaderData *header, ContentData *dest, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::CommandMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

  static std::vector<std::string> GetAvailableCommandNames();
};

#endif // IGTLIOCOMMANDCONVERTER_H
