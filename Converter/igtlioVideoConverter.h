/*==========================================================================

 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
 Version:   $Revision: 15552 $

 ==========================================================================*/

#ifndef IGTLIOVIDEOCONVERTER_H
#define IGTLIOVIDEOCONVERTER_H

#include "igtlioConverterExport.h"


// OpenIGTLink includes
#include "igtlVideoMessage.h"
#include "igtlCodecCommonClasses.h"
#include "igtlConfigure.h"
#include "igtl_util.h"
#include "igtlI420Encoder.h"
#include "igtlI420Decoder.h"
#if defined(OpenIGTLink_USE_H264)
  #include "igtlH264Encoder.h"
  #include "igtlH264Decoder.h"
#endif
#if defined(OpenIGTLink_USE_AV1)
  #include "igtlAV1Encoder.h"
  #include "igtlAV1Decoder.h"
#endif
#if defined(OpenIGTLink_USE_VP9)
  #include "igtlVP9Encoder.h"
  #include "igtlVP9Decoder.h"
#endif
#if defined(OpenIGTLink_USE_OpenHEVC)
  #include "igtlH265Decoder.h"
#endif
#if defined(OpenIGTLink_USE_X265)
  #include "igtlH265Encoder.h"
#endif
#include "igtlioBaseConverter.h"

class vtkImageData;
class vtkMatrix4x4;
class vtkUnsignedCharArray;

using namespace igtl;

/** Conversion between igtl::VideoMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT igtlioVideoConverter : public igtlioBaseConverter
{
public:
  igtlioVideoConverter();
  /**
   * This structure contains everything that igtl::VideoMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
    vtkSmartPointer<vtkImageData> image;
    VideoFrameType frameType;
    char codecName[IGTL_VIDEO_CODEC_NAME_SIZE];
    VideoMessage::Pointer videoMessage; // for saving the compressed data.
    VideoMessage::Pointer keyFrameMessage; // for saving the compressed data.
    bool keyFrameUpdated;
    vtkSmartPointer<vtkMatrix4x4> transform; // ijk2ras, From image pixel space to RAS
    vtkSmartPointer<vtkUnsignedCharArray> frameData;
    bool grayscale;
  };

  static const char*  GetIGTLName() { return GetIGTLTypeName(); }
  static const char* GetIGTLTypeName() { return "VIDEO"; }

  static int fromIGTL(MessageBase::Pointer source, HeaderData* header, ContentData* content, std::map<std::string, GenericDecoder*> decoders, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo);
  static int toIGTL(const HeaderData& header, const ContentData& source, GenericEncoder* encoder, igtl::MessageBase::MetaDataMap metaInfo = igtl::MessageBase::MetaDataMap());

  static int IGTLFrameToVTKTransform(igtl::VideoMessage::Pointer videoMsg, vtkSmartPointer<vtkMatrix4x4> ijk2ras);
  static int VTKTransformToIGTLFrame(const vtkMatrix4x4& ijk2ras, int imageSize[3], double spacing[3], double origin[3], igtl::VideoMessage::Pointer videoMsg);

protected:

  static int IGTLToVTKScalarType(int igtlType);
  static int IGTLToVTKImageData(ContentData *dest, VideoMessage::Pointer videoMessage, GenericDecoder * videoStreamDecoder);

};

#endif //IGTLIOVIDEOCONVERTER_H
