/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __igtlPolyDataConverter_h
#define __igtlPolyDataConverter_h

#include "igtlSupportModuleConverterExport.h"

// OpenIGTLink includes
#include <igtlPolyDataMessage.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkCellArray;
class vtkDataSetAttributes;

#include "igtlLightObject.h"

namespace igtl
{

/** Conversion between igtl::PolyDataMessage and vtk classes.
 *
 */
class OPENIGTLINK_SUPPORT_MODULE_CONVERTER_EXPORT PolyDataConverter : public LightObject
{
public:
  /** Standard class typedefs. */
  typedef PolyDataConverter        Self;
  typedef LightObject              Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  igtlNewMacro(Self);

  /** Run-time type information (and related methods). */
  igtlTypeMacro(PolyDataConverter, LightObject);

  /**
   * This structure contains everything that igtl::PolyMessage is able to contain,
   * in a vtk-friendly format.
   */
  struct MessageContent
  {
    vtkSmartPointer<vtkPolyData> polydata;
    std::string deviceName;
  };

  virtual void PrintSelf(std::ostream& os) const;

  virtual const char*  GetIGTLName() { return "POLYDATA"; };

  virtual int IGTLToVTK(igtl::MessageBase::Pointer source, MessageContent* dest, bool checkCRC);
  virtual int VTKToIGTL(const MessageContent& source, igtl::PolyDataMessage::Pointer* dest);

protected:
  PolyDataConverter();
  ~PolyDataConverter();

private:
  // Extract vtkPolyData from existing polyDataMsg, insert into existing poly.
  int IGTLToVTKPolyData(PolyDataMessage::Pointer polyDataMsg, vtkSmartPointer<vtkPolyData> poly);
  // Insert an existing vtkPolyData into a cleared PolyDataMessage.
  int VTKPolyDataToIGTL(vtkSmartPointer<vtkPolyData> poly, PolyDataMessage::Pointer outMessage);

  // Utility function for MRMLToIGTL(): Convert vtkCellArray to igtl::PolyDataCellArray
  int VTKToIGTLCellArray(vtkCellArray* src, igtl::PolyDataCellArray* dest);

  // Utility function for MRMLToIGTL(): Convert i-th vtkDataSetAttributes (vtkCellData and vtkPointData)
  // to igtl::PolyDataAttribute
  int VTKToIGTLAttribute(vtkDataSetAttributes* src, int i, igtl::PolyDataAttribute* dest);
};

}


#endif //__igtlPolyDataConverter_h
