/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __igtlTransformConverter_h
#define __igtlTransformConverter_h

// OpenIGTLink includes
#include <igtlTransformMessage.h>

class vtkMatrix4x4;
#include "igtlioConverterExport.h"
#include "igtlBaseConverter.h"

namespace igtl
{

/** Conversion between igtl::TransformMessage and vtk classes.
 *
 */
class OPENIGTLINKIO_CONVERTER_EXPORT TransformConverter : public BaseConverter
{
public:
 /** Standard class typedefs. */
 typedef TransformConverter       Self;
 typedef BaseConverter            Superclass;
 typedef SmartPointer<Self>       Pointer;
 typedef SmartPointer<const Self> ConstPointer;

 /** Method for creation through the object factory. */
 igtlNewMacro(Self);

 /** Run-time type information (and related methods). */
 igtlTypeMacro(TransformConverter, BaseConverter);

  /**
   * This structure contains everything that igtl::ImageMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct ContentData
  {
  vtkSmartPointer<vtkMatrix4x4> transform;
  std::string deviceName;
  };

  virtual void PrintSelf(std::ostream& os) const;

  virtual const char*  GetIGTLName() { return GetIGTLTypeName(); };
  static const char* GetIGTLTypeName() { return "TRANSFORM"; };

  int fromIGTL(igtl::MessageBase::Pointer source, HeaderData* header, ContentData* content, bool checkCRC);
  int toIGTL(const HeaderData& header, const ContentData& source, igtl::TransformMessage::Pointer* dest);

protected:
  TransformConverter();
  ~TransformConverter();
};

}


#endif //__igtlTransformConverter_h
