/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOTRANSFORMDEVICE_H
#define IGTLIOTRANSFORMDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioTransformConverter.h"
#include "igtlioDevice.h"

class vtkImageData;

typedef vtkSmartPointer<class igtlioTransformDevice> igtlioTransformDevicePointer;

/// A Device supporting the IMAGE igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT igtlioTransformDevice : public igtlioDevice
{
public:
  enum {
    TransformModifiedEvent         = 118957,
  };

 virtual unsigned int GetDeviceContentModifiedEvent() const VTK_OVERRIDE;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(igtlioTransformConverter::ContentData content);
  igtlioTransformConverter::ContentData GetContent();

public:
  static igtlioTransformDevice *New();
  vtkTypeMacro(igtlioTransformDevice,igtlioDevice);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioTransformDevice();
  ~igtlioTransformDevice();

 protected:
  igtl::TransformMessage::Pointer OutTransformMessage;
  igtl::GetTransformMessage::Pointer GetTransformMessage;

  igtlioTransformConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT igtlioTransformDeviceCreator : public igtlioDeviceCreator
{
public:
  virtual igtlioDevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static igtlioTransformDeviceCreator *New();
  vtkTypeMacro(igtlioTransformDeviceCreator,vtkObject);
};

#endif //IGTLIOTRANSFORMDEVICE_H
