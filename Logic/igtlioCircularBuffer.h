/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 ==========================================================================*/

#ifndef IGTLIOCIRCULARBUFFER_H
#define IGTLIOCIRCULARBUFFER_H

#include <string>
#include <vtkObject.h>
#include <igtlMessageBase.h>
#include "igtlioLogicExport.h"

#define IGTLCB_CIRC_BUFFER_SIZE    3

class vtkMutexLock;

namespace igtlio
{
  
  class OPENIGTLINKIO_LOGIC_EXPORT CircularBuffer : public vtkObject
  {
  public:
  
  static CircularBuffer *New();
  vtkTypeMacro(CircularBuffer,vtkObject);
  
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
  CircularBuffer();
  virtual ~CircularBuffer();
  
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
  
} // namespace igtlio

#endif //IGTLIOCIRCULARBUFFER_H