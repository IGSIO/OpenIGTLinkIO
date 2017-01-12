/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLIOTransformDevice_h
#define __vtkIGTLIOTransformDevice_h

// igtlio includes
#include "igtlioDevicesExport.h"

#include "igtlTransformConverter.h"
#include "vtkIGTLIODevice.h"

// OpenIGTLink includes
#include <igtlImageMessage.h>

// VTK includes
#include <vtkObject.h>

class vtkImageData;

namespace igtlio {

typedef vtkSmartPointer<class vtkIGTLIOTransformDevice> vtkIGTLIOTransformDevicePointer;

/// A Device supporting the IMAGE igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOTransformDevice : public vtkIGTLIODevice
{
public:
 virtual std::string GetDeviceType() const;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

  void SetContent(igtlio::TransformConverter::ContentData content);
  igtlio::TransformConverter::ContentData GetContent();

public:
  static vtkIGTLIOTransformDevice *New();
  vtkTypeMacro(vtkIGTLIOTransformDevice,vtkIGTLIODevice);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkIGTLIOTransformDevice();
  ~vtkIGTLIOTransformDevice();

 protected:
  igtl::TransformMessage::Pointer OutTransformMessage;
  igtl::GetTransformMessage::Pointer GetTransformMessage;

  igtlio::TransformConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOTransformDeviceCreator : public vtkIGTLIODeviceCreator
{
public:
  virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOTransformDeviceCreator *New();
  vtkTypeMacro(vtkIGTLIOTransformDeviceCreator,vtkObject);
};

}


#endif //__vtkIGTLIOTransformDevice_h
