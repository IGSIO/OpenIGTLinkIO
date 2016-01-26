/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLIOImageDevice_h
#define __vtkIGTLIOImageDevice_h

// igtlio includes
#include "igtlioDevicesExport.h"

#include "igtlImageConverter.h"
#include "vtkIGTLIODevice.h"

// OpenIGTLink includes
#include <igtlImageMessage.h>

// VTK includes
#include <vtkObject.h>

class vtkImageData;

/// implements smart pointer conversion for igtl::SmartPointer,
/// similar to std::dynamic_pointer_cast
/// TODO: Move this into igtlSmartPointer.h
template <class T, class U>
igtl::SmartPointer<T> dynamic_pointer_cast(const igtl::SmartPointer<U>& sp) //noexcept
{
 T* ptr = dynamic_cast<T*>(sp.GetPointer());
 return igtl::SmartPointer<T>(ptr);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class vtkIGTLIOImageDeviceCreator : public vtkIGTLIODeviceCreator
{
public:
  virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name);
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOImageDeviceCreator *New();
  vtkTypeMacro(vtkIGTLIOImageDeviceCreator,vtkObject);
};

//---------------------------------------------------------------------------
/// A Device supporting the IMAGE igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIOImageDevice : public vtkIGTLIODevice
{
public:
 virtual std::string GetDeviceType() const;
 virtual std::string GetDeviceName() const;
  /// Set device name.
  /// Caution: Changing the device name of a device registered in
  /// a vtkIGTLIOConnector will cause undefined behaviour.
  virtual void SetDeviceName(std::string name);
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC);
 virtual igtl::MessageBase::Pointer GetIGTLMessage();
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix);
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const;

 public:
  static vtkIGTLIOImageDevice *New();
  vtkTypeMacro(vtkIGTLIOImageDevice,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkIGTLIOImageDevice();
  ~vtkIGTLIOImageDevice();

 protected:
  igtl::ImageMessage::Pointer OutImageMessage;
  igtl::GetImageMessage::Pointer GetImageMessage;

  igtl::ImageConverter::MessageContent Content;
  igtl::ImageConverter::Pointer Converter;
};


#endif //__vtkIGTLIOImageDevice_h
