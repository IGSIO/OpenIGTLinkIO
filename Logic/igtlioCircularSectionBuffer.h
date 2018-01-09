/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#ifndef IGTLIOCIRCULARSECTIONBUFFER_H
#define IGTLIOCIRCULARSECTIONBUFFER_H

#include <string>
#include <vtkObject.h>
#include <igtlMessageBase.h>
#include "igtlioLogicExport.h"

#define IGTLCB_CIRC_BUFFER_SIZE                3
#define IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM   50

class vtkMutexLock;

namespace igtlio
{
  
class OPENIGTLINKIO_LOGIC_EXPORT CircularSectionBuffer : public vtkObject
{
 public:

  static CircularSectionBuffer *New();
  vtkTypeMacro(CircularSectionBuffer,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  int GetNumberOfBuffer() { return this->BufferSize; }

  int            StartPush();
  void           EndPush();
  igtl::MessageBase::Pointer GetPushBuffer();
  
  int            StartPull();
  void           EndPull();
  igtl::MessageBase::Pointer GetPullBuffer();
  
  int            StartSectionPull();
  igtl::MessageBase::Pointer GetPullSectionBuffer();
  void           EndSectionPull();

  int            IsUpdated() { return this->UpdateFlag; };
  
  int            Initialization();
  
  enum
  {
  SinglePacketMode = 0,
  MultiplePacketsMode = 1
  };
  
  enum
  {
  DataFilled = 2,
  DataEmpty = 3,
  DataProcessed =4
  };
  
  vtkSetMacro( PacketMode, int);
  vtkGetMacro( PacketMode, int);
  
  vtkSetMacro( BufferSize, int);
  vtkGetMacro( BufferSize, int);
  
  bool IsSectionBufferInProcess();
  

 protected:
  CircularSectionBuffer();
  virtual ~CircularSectionBuffer();

 protected:

  vtkMutexLock*      Mutex;
  int                Last;        // updated by connector thread
  int                First;       // updated by connector thread
  int                InPush;      // updated by connector thread
  int                InUseBegin;       // updated by main thread
  int                InUseEnd;       // updated by main thread

  int                UpdateFlag;  // non-zero if updated since StartPull() has called
  
  int                PacketMode;
  
  int                BufferSize;
  
#if OpenIGTLink_ENABLE_VIDEOSTREAMING
  std::string       DeviceType[IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM];
  unsigned short     DataStatus[IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM];
  igtl::MessageBase::Pointer Messages[IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM];
#else
  std::string       DeviceType[IGTLCB_CIRC_BUFFER_SIZE];
  unsigned short    DataStatus[IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM];
  igtl::MessageBase::Pointer Messages[IGTLCB_CIRC_BUFFER_SIZE];
#endif

};

} // namespace igtlio

#endif //IGTLIOCIRCULARSECTIONBUFFER_H
