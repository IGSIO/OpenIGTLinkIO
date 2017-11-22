/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLDevice.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef IGTLIODEVICE_H
#define IGTLIODEVICE_H

#include <vtkCommand.h>
#include <vtkIntArray.h>
#include <set>

#include "igtlioDevicesExport.h"
#include "igtlioBaseConverter.h"


namespace igtlio
{
typedef vtkSmartPointer<class Device> DevicePointer;

/// A vtkIGTLIODevice represents one device connected over OpenIGTLink.
///
/// The Device has a specific type, e.g. Image, Transform..., and
/// is capable of receiving and emitting igtl messages for that type.
///
/// One Device is bound to a device name, corresponding to an igtl device name.
///
class OPENIGTLINKIO_DEVICES_EXPORT Device : public vtkObject
{
public:
  enum MESSAGE_DIRECTION {
    MESSAGE_DIRECTION_IN,
    MESSAGE_DIRECTION_OUT,
    NUM_MESSAGE_DIRECTION,
  };

 enum MESSAGE_PREFIX {
   MESSAGE_PREFIX_NOT_DEFINED,
   //MESSAGE_PREFIX_GET,
   //MESSAGE_PREFIX_START,
   //MESSAGE_PREFIX_STOP,
   MESSAGE_PREFIX_RTS,
   NUM_MESSAGE_PREFIX,
 };

 enum {
   ReceiveEvent          = 118948,
   ResponseEvent         = 118952,
   ModifiedEvent         = vtkCommand::ModifiedEvent,

   CommandReceivedEvent    = 119001, // COMMAND device got a query, COMMAND received
   CommandResponseReceivedEvent = 119002  // COMMAND device got a response, RTS_COMMAND received
 };


public:
 
 virtual unsigned int GetDeviceContentModifiedEvent() const;
  
 virtual std::string GetDeviceType() const;

 virtual std::string GetDeviceName() const;
  /// Set device name.
  /// Caution: Changing the device name of a device registered in
  /// a vtkIGTLIOConnector will cause undefined behaviour.
  virtual void SetDeviceName(std::string name);

  BaseConverter::HeaderData GetHeader();

 vtkSetMacro( PushOnConnect, bool );
 vtkGetMacro( PushOnConnect, bool );
 vtkSetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkGetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkSetMacro( Visibility, bool );
 vtkGetMacro( Visibility, bool );

 virtual double GetTimestamp() const;
 virtual void SetTimestamp(double val);

 void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

 bool MessageDirectionIsOut() const { return MessageDirection==MESSAGE_DIRECTION_OUT; }
 bool MessageDirectionIsIn() const { return MessageDirection==MESSAGE_DIRECTION_IN; }

 // Read the content of buffer and update the device accordingly.
 // This assumes the incoming message has a device_type corresponding to this device
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) = 0;

 // Write the Device content into buffer.
 // The returned pointer might be allocated internally once
 // and reused between successive calls.
 virtual igtl::MessageBase::Pointer GetIGTLMessage() = 0;

 // As GetIGTLMessage(), except that the generated message is for the given prefix.
 // TODO: merge with above
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) { return igtl::MessageBase::Pointer(); }

 // Return supported messages, in the set GET, STT, STP, RTS,
 // The default message equals GetDeviceType() and is always supported,
 // prefixed messages are <prefix>_<device_type>.
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const { return std::set<MESSAGE_PREFIX>(); }

 // TODO: add old features from Connector:
 //       - lock (means dont accept incoming messages),
 //       - gettimestamp (of last incoming message)
 
 // Return Meta information
 const igtl::MessageBase::MetaDataMap& GetMetaData() const;
 
 /// Add Meta data element
 bool SetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE encodingScheme, std::string value);

 template <class dataType>
 bool SetMetaDataElement(const std::string& key, dataType value)
 {
   std::stringstream ss;
   ss << value;
   return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
 }
 
 /// Get meta data element
 bool GetMetaDataElement(const std::string& key, std::string& value) const;
 bool GetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE& encoding, std::string& value) const;
 
 /// Clear all data elements
 void ClearMetaData();

public:
  vtkAbstractTypeMacro(Device,vtkObject);

protected:
  void SetHeader(BaseConverter::HeaderData header);

  BaseConverter::HeaderData HeaderData;
  
  igtl::MessageBase::MetaDataMap metaInfo;

protected:
 Device();
 virtual ~Device();

private:
 MESSAGE_DIRECTION MessageDirection;
 bool PushOnConnect;
 bool Visibility;

};

//---------------------------------------------------------------------------

class OPENIGTLINKIO_DEVICES_EXPORT DeviceCreator : public vtkObject
{
public:
  // Create an instance of the specific device, with the given device_id
  virtual DevicePointer Create(std::string device_name) = 0;
  // Return the device_type this factory creates devices for.
  virtual std::string GetDeviceType() const = 0;
  vtkAbstractTypeMacro(DeviceCreator,vtkObject);
};

} // namespace igtlio


#endif //IGTLIODEVICE_H
