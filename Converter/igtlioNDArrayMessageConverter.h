#ifndef NDARRAYCONVERTER_H
#define NDARRAYCONVERTER_H

#include "igtlioConverterExport.h"
#include <igtlNDArrayMessage.h>
#include "igtlioBaseConverter.h"
class OPENIGTLINKIO_CONVERTER_EXPORT igtlioNDArrayMessageConverter : public igtlioBaseConverter
{
public:


    struct ContentData
    {
    igtl::NDArrayMessage NDArray_msg;
    };

    static const char* GetIGTLName() { return GetIGTlTypeName(); }
    static const char* GetIGTlTypeName() {return "ARRAY"; }

    static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
    static int toIGTL(const HeaderData& header, const ContentData& source, igtl::NDArrayMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif //NDARRAYCONVERTER_H