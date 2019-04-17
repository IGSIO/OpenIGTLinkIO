#ifndef IMAGEMETACONVERTER_H
#define IMAGEMETACONVERTER_H

// OpenIGTLinkIO includes
#include "igtlioBaseConverter.h"
#include "igtlioConverterExport.h"

// OpenIGTLink includes
#include <igtlImageMetaMessage.h>

class OPENIGTLINKIO_CONVERTER_EXPORT igtlioImageMetaConverter : public igtlioBaseConverter
{
public:
  /**
   * Content of STRING message.
   */
  struct ImageMetaElement
  {
    std::string   Name;        /* name / description (< 64 bytes)*/
    std::string   DeviceName;  /* device name to query the IMAGE and COLORT */
    std::string   Modality;    /* modality name (< 32 bytes) */
    std::string   PatientName; /* patient name (< 64 bytes) */
    std::string   PatientID;   /* patient ID (MRN etc.) (< 64 bytes) */
    double        Timestamp;   /* scan time */
    int           Size[3];     /* entire image volume size */
    unsigned char ScalarType;  /* scalar type. see scalar_type in IMAGE message */
  };
  typedef std::vector<ImageMetaElement> ImageMetaDataList;

  struct ContentData
  {
    ImageMetaDataList ImageMetaDataElements;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "IMGMETA"; }

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::ImageMetaMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

};

#endif // IMAGEMETACONVERTER_H
