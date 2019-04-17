#ifndef LABELMETACONVERTER_H
#define LABELMETACONVERTER_H

// OpenIGTLinkIO includes
#include "igtlioBaseConverter.h"
#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlLabelMetaMessage.h>

class OPENIGTLINKIO_CONVERTER_EXPORT igtlioLabelMetaConverter : public igtlioBaseConverter
{
public:
  /**
   * Content of STRING message.
   */
  struct LabelMetaElement
  {
    std::string   Name;        /* Name or description of the image */
    std::string   DeviceName;  /* ID to query the IMAGE */
    unsigned char Label;       /* Label of the structure (0 if unused) */
    unsigned char RGBA[4];     /* Color in RGBA (0 0 0 0 if no color is defined) */
    int           Size[3];     /* Number of pixels in each direction (same as in IMAGE), bounding box of the structure(s) */
    std::string   Owner;       /* ID of the owner image/sliceset. Voxel objects from different slicesets can be sent if
                               slicesets are fused. Can be empty if n/a */
  };
  typedef std::vector<LabelMetaElement> LabelMetaDataList;

  struct ContentData
  {
    LabelMetaDataList LabelMetaDataElements;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "LBMETA"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::LabelMetaMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif // IMAGEMETACONVERTER_H
