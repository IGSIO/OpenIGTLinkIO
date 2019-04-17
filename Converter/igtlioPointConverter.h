#ifndef POINTCONVERTER_H
#define POINTCONVERTER_H

// OpenIGTLInkIO includes
#include "igtlioBaseConverter.h"
#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlPointMessage.h>

class OPENIGTLINKIO_CONVERTER_EXPORT igtlioPointConverter : public igtlioBaseConverter
{
public:
  /**
   * Content of STRING message.
   */
  struct PointElement
  {
    std::string   Name; /* name / description (< 64 bytes) */
    std::string   GroupName; /* Can be "Labeled Point", "Landmark", Fiducial", ... */
    igtlUint8     RGBA[4]; /* Color in R/G/B/A */
    igtlFloat32   Position[3]; /* Position */
    igtlFloat32   Radius; /* Radius of the point. Can be 0.*/
    std::string   Owner; /* Device name of the ower image */
  };
  typedef std::vector<PointElement> PointList;

  struct ContentData
  {
    PointList PointElements;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "POINT"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::PointMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif // IMAGEMETACONVERTER_H
