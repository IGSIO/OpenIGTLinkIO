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
#include <igtlVideoMessage.h>
#include "igtlVideoMessage.h"
#if OpenIGTLink_LINK_H264
  #include "H264Encoder.h"
  #include "H264Decoder.h"
#endif
#if OpenIGTLink_LINK_VP9
  #include "VP9Encoder.h"
  #include "VP9Decoder.h"
#endif
#if OpenIGTLink_LINK_X265
  #include "H265Encoder.h"
  #include "H265Decoder.h"
#endif

#include "igtlioBaseConverter.h"

class vtkImageData;
class vtkMatrix4x4;

namespace igtlio
{
  
  /** Conversion between igtl::VideoMessage and vtk classes.
   *
   */
  class OPENIGTLINKIO_CONVERTER_EXPORT VideoConverter : public BaseConverter
  {
  public:
    VideoConverter();
    /**
     * This structure contains everything that igtl::VideoMessage is able to contain,
     * in a vtk-friendly format.
     */
    struct ContentData
    {
      vtkSmartPointer<vtkImageData> image;
    };
    
    static const char*  GetIGTLName() { return GetIGTLTypeName(); }
    static const char* GetIGTLTypeName() { return "VIDEO"; }
    
    static int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, GenericDecoder* decoder, bool checkCRC);
    static int toIGTL(const HeaderData& header, const ContentData& source, igtl::VideoMessage::Pointer* dest, GenericEncoder* encoder);
    
  protected:
    
    static int IGTLToVTKScalarType(int igtlType);
    static int IGTLToVTKImageData(igtl::VideoMessage::Pointer videoMsg, ContentData *dest, GenericDecoder * videoStreamDecoder);

  };
  
} //namespace igtlio


#endif //IGTLIOVIDEOCONVERTER_H
