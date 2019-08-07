/*==========================================================================

Portions (c) Copyright 2019 Robarts Research Institute All Rights Reserved.

==========================================================================*/

#ifndef IGTLIOTDATACONVERTER_H
#define IGTLIOTDATACONVERTER_H

// Local includes
#include "igtlioConverterExport.h"
#include "igtlioBaseConverter.h"

// OpenIGTLink includes
#include <igtlTrackingDataMessage.h>

// STL includes
#include <map>

class vtkMatrix4x4;

/** Conversion between igtl::TDATAMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT igtlioTrackingDataConverter : public igtlioBaseConverter
{
public:
  /**
   * This structure contains everything that igtl::TDATAMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentEntry
  {
    ContentEntry() {}
    ContentEntry(vtkSmartPointer<vtkMatrix4x4> _transform, std::string _deviceName, std::string _transformName)
      : transform(_transform)
      , deviceName(_deviceName)
      , transformName(_transformName) {}
    vtkSmartPointer<vtkMatrix4x4> transform;
    std::string deviceName;
    std::string transformName;
  };
  struct ContentData
  {
    std::map<int, ContentEntry> trackingDataElements;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); };
  static const char* GetIGTLTypeName() { return "TDATA"; };

  static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, igtl::TrackingDataMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

  static int IGTLToVTKTDATA(const igtl::Matrix4x4& igtlTDATA, vtkSmartPointer<vtkMatrix4x4> vtkTDATA);
  static int VTKToIGTLTDATA(const vtkMatrix4x4& vtkTDATA, igtl::Matrix4x4& igtlTDATA);

private:
  static int IGTLHeaderToTDATAInfo(igtl::MessageBase::Pointer source, ContentData* dest);
  static int TDATAMetaDataToIGTL(const ContentData& source, igtl::MessageBase::Pointer* dest);

};

#endif //IGTLIOTDATACONVERTER_H
