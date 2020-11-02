/*==========================================================================

 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 ==========================================================================*/

#ifndef IGTLIOCIRCULARBUFFER_H
#define IGTLIOCIRCULARBUFFER_H

// STL includes
#include <string>

// VTK includes
#include <vtkObject.h>

// OpenIGTLink includes
#include <igtlMessageBase.h>

// Local includes
#include "igtlioLogicExport.h"

#define IGTLCB_CIRC_BUFFER_SIZE    3

#ifndef VTK_OVERRIDE
  #define VTK_OVERRIDE override
#endif

class vtkMutexLock;

class OPENIGTLINKIO_LOGIC_EXPORT igtlioCircularBuffer : public vtkObject
{
public:
  static igtlioCircularBuffer *New();
  vtkTypeMacro(igtlioCircularBuffer, vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  int GetNumberOfBuffer() { return IGTLCB_CIRC_BUFFER_SIZE; }

  int            StartPush();
  void           EndPush();
  igtl::MessageBase::Pointer GetPushBuffer();

  int            StartPull();
  void           EndPull();
  igtl::MessageBase::Pointer GetPullBuffer();

  int            IsUpdated() { return this->UpdateFlag; };

protected:
    igtlioCircularBuffer();
    virtual ~igtlioCircularBuffer();

protected:

  vtkMutexLock*      Mutex;
  int                Last;        // updated by connector thread
  int                InPush;      // updated by connector thread
  int                InUse;       // updated by main thread

  int                UpdateFlag;  // non-zero if updated since StartPull() has called

  std::string        DeviceType[IGTLCB_CIRC_BUFFER_SIZE];
  long long          Size[IGTLCB_CIRC_BUFFER_SIZE];
  unsigned char*     Data[IGTLCB_CIRC_BUFFER_SIZE];

  igtl::MessageBase::Pointer Messages[IGTLCB_CIRC_BUFFER_SIZE];

};

#endif //IGTLIOCIRCULARBUFFER_H
