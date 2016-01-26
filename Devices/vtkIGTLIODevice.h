/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLDevice.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLIODevice_h
#define __vtkIGTLIODevice_h

// igtlio includes
#include "igtlioDevicesExport.h"

// OpenIGTLink includes
#include <igtlMessageBase.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <vector>
#include <string>
#include <set>

class vtkIGTLIODevice;

class vtkIGTLIODeviceCreator : public vtkObject
{
public:
 // Create an instance of the specific device, with the given device_id
 virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name) = 0;
 // Return the device_type this factory creates devices for.
 virtual std::string GetDeviceType() const = 0;

  vtkAbstractTypeMacro(vtkIGTLIODeviceCreator,vtkObject);

};

/// A vtkIGTLIODevice represents one device connected over OpenIGTLink.
///
/// The Device has a specific type, e.g. Image, Transform..., and
/// is capable of receiving and emitting igtl messages for that type.
///
/// One Device is bound to a device name, corresponding to an igtl device name.
///
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIODevice : public vtkObject
{
public:
  enum MESSAGE_DIRECTION {
    MESSAGE_DIRECTION_IN,
    MESSAGE_DIRECTION_OUT,
    NUM_MESSAGE_DIRECTION,
  };

 enum MESSAGE_PREFIX {
   MESSAGE_PREFIX_NOT_DEFINED,
   MESSAGE_PREFIX_GET,
   MESSAGE_PREFIX_START,
   MESSAGE_PREFIX_STOP,
   NUM_MESSAGE_PREFIX,
 };
 enum QUERY_STATUS {
   QUERY_STATUS_NONE,
   QUERY_STATUS_WAITING,
   QUERY_STATUS_SUCCESS,
   QUERY_STATUS_EXPIRED,
   QUERY_STATUS_ERROR,
   NUM_QUERY_STATUS,
 };
 enum {
   ConnectedEvent        = 118944,
   DisconnectedEvent     = 118945,
   ActivatedEvent        = 118946,
   DeactivatedEvent      = 118947,
   ReceiveEvent          = 118948,
   NewDeviceEvent        = 118949,
   DeviceModifiedEvent   = 118950,
   RemovedDeviceEvent    = 118951,
   ResponseEvent         = 118952,
 };


public:
 virtual std::string GetDeviceType() const = 0;
 virtual std::string GetDeviceName() const = 0;

 vtkSetMacro( PushOnConnect, bool );
 vtkGetMacro( PushOnConnect, bool );
 vtkSetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkGetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkSetMacro( QueryTimeOut, double );
 vtkGetMacro( QueryTimeOut, double );

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

 // TODO: add ole features from Connector:
 //       - lock (means dont accept incoming messages),
 //       - gettimestamp (of last incoming message)

 // TODO: notify listeners when:
 // created, deleted, modified
 // This can be done from the Connector or Logic instead, this would enable all 3 signals at the same level.

 /// Query handling:
 /// Each device has a list of queries (GET_, STT_, STP_) that has been sent
 /// and are awaiting reply.
 //
 /// Device::GetMessage() pushes a query,
 /// Device::ReceiveMessage() processes the reply, and emits events for the receive
 ///   - statechange: waiting, success, expired,...
 //
 //
 /// One query (GET_, STT_ or STP_-message that requires an answer)
 struct QueryType
 {
   std::string query_message;
   std::string response_message;
   double query_timestamp;
   QUERY_STATUS status;
 };

 /// Get all current queries
  std::vector<QueryType> GetQueries() const;
  /// check for waiting queries that have waited beoynd the timeout for an answer, mark them as expired.
  int CheckQueryExpiration();
  /// remove all queries that are answered or expired.
  int PruneCompletedQueries();
  int CancelQuery(int index);


 public:

//  // IGTL to MRML Converter types (returned values from GetConverterType())
//  // NOTE: if you want to define a child class that can handle multiple types
//  // of OpenIGTLink messages, override GetConverterType() method to return
//  // TYPE_MULTI_IGTL_NAME.
//  enum {
//    TYPE_NORMAL,            // supports only single IGTL message type (default)
//    TYPE_MULTI_IGTL_NAMES,  // supports multiple IGTL message names (device types)
// };

 public:

//  static vtkIGTLIODevice *New();
  vtkTypeMacro(vtkIGTLIODevice,vtkObject);

private:
 std::vector<QueryType> Queries;
 MESSAGE_DIRECTION MessageDirection;
 bool PushOnConnect;
 double QueryTimeOut;

//  void PrintSelf(ostream& os, vtkIndent indent);

//  virtual int          GetConverterType() { return TYPE_NORMAL; };

//  // IGTL Device / MRML Tag names
//  virtual const char*  GetIGTLName()      { return NULL;};
//  virtual const char*  GetMRMLName()      { return NULL;};

//  // Following functions are implemented only if exists in OpenIGTLink specification
//  virtual const char*  GetIGTLStartQueryName() { return NULL; };
//  virtual const char*  GetIGTLStopQueryName()  { return NULL; };
//  virtual const char*  GetIGTLGetQueryName()   { return NULL; };
//  virtual const char*  GetIGTLStatusName()     { return NULL; };

//  // Description:
//  // GetNodeEvents() returns a list of events, which an IGTLConnector should react to.
//  // The first element should be an event to export data, although multiple events can be defined.
//  virtual vtkIntArray* GetNodeEvents()    { return NULL; };

//  // This simpler call exists when the message is not available to provide more information in the function
//  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* vtkNotUsed(scene), const char* vtkNotUsed(name))
//  { return NULL; };
//  // This call enables the created node to query the message to determine any necessary properties
//  virtual vtkMRMLNode* CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer vtkNotUsed(message))
//  { return this->CreateNewNode(scene, name); };

//  // for TYPE_MULTI_IGTL_NAMES
//  int                  GetNumberOfIGTLNames()   { return this->IGTLNames.size(); };
//  const char*          GetIGTLName(int index)   { return this->IGTLNames[index].c_str(); };

//  // Description:
//  // Functions to convert OpenIGTLink message to MRML node.
//  // If mrmlNode is QueryNode, the function will generate query node. (event is not used.)
//  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer,
//                                  vtkMRMLNode* node);

//  // Description:
//  // Functions to generate an OpenIGTLink message
//  // If mrmlNode is QueryNode, the function will generate query node. (event is not used.)
//  virtual int          MRMLToIGTL(unsigned long vtkNotUsed(event), vtkMRMLNode* vtkNotUsed(mrmlNode),
//                                  int* vtkNotUsed(size), void** vtkNotUsed(igtlMsg)){ return 0; };

//  // Check query que (called periodically by timer)
//  // (implemeted only if ncessary)
//  virtual int CheckQueryQue(double vtkNotUsed(ctime)) { return true; }

//  vtkGetMacro( CheckCRC, int );
//  vtkSetMacro( CheckCRC, int );

//  // Set/Get pointer to OpenIGTlinkIFLogic
//  void SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic* logic);
//  vtkSlicerOpenIGTLinkIFLogic* GetOpenIGTLinkIFLogic();

//  // Visualization
//  // If an MRML node for this converter type can be visualized,
//  // the following functions must be implemented.
//  virtual int IsVisible() { return 0; };
//  virtual void SetVisibility(int vtkNotUsed(sw),
//                             vtkMRMLScene * vtkNotUsed(scene),
//                             vtkMRMLNode * vtkNotUsed(node)) {};

 protected:
  vtkIGTLIODevice();
  ~vtkIGTLIODevice();

 protected:

//  // list of IGTL names (used only when the class supports multiple IGTL names)
//  std::vector<std::string>  IGTLNames;

//  int CheckCRC;

//  vtkIGTLIODevicePrivate* Private;
};


#endif //__vtkIGTLIODevice_h
