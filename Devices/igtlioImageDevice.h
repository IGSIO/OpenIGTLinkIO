/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIOIMAGEDEVICE_H
#define IGTLIOIMAGEDEVICE_H

#include "igtlioDevicesExport.h"

#include "igtlioImageConverter.h"
#include "igtlioDevice.h"


class vtkImageData;

namespace igtlio
{

typedef vtkSmartPointer<class ImageDevice> ImageDevicePointer;

/// A Device supporting the IMAGE igtl Message.
class OPENIGTLINKIO_DEVICES_EXPORT ImageDevice : public Device
{
public:
  
  enum {
    ImageModifiedEvent         = 118955,
  };
  
 virtual vtkIntArray* GetDeviceContentModifiedEvent() const VTK_OVERRIDE ;
 virtual std::string GetDeviceType() const VTK_OVERRIDE;
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage() VTK_OVERRIDE;
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) VTK_OVERRIDE;
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const VTK_OVERRIDE;

  void SetContent(ImageConverter::ContentData content);
  ImageConverter::ContentData GetContent();

public:
  static ImageDevice *New();
  vtkTypeMacro(ImageDevice,Device);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  ImageDevice();
  ~ImageDevice();

 protected:
  igtl::ImageMessage::Pointer OutImageMessage;
  igtl::GetImageMessage::Pointer GetImageMessage;

  ImageConverter::ContentData Content;
};

//---------------------------------------------------------------------------
class OPENIGTLINKIO_DEVICES_EXPORT ImageDeviceCreator : public DeviceCreator
{
public:
  virtual DevicePointer Create(std::string device_name) VTK_OVERRIDE;
  virtual std::string GetDeviceType() const VTK_OVERRIDE;

  static ImageDeviceCreator *New();
  vtkTypeMacro(ImageDeviceCreator,vtkObject);
};

} //namespace igtlio

#endif //IGTLIOIMAGEDEVICE_H
