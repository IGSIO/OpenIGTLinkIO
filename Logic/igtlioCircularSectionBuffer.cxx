/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

// VTK includes
#include "igtlioCircularSectionBuffer.h"

#include <vtkObjectFactory.h>
#include <vtkMutexLock.h>
#include <vtksys/SystemTools.hxx>

// OpenIGTLink includes
#include <igtlMessageBase.h>

// STD includes
#include <string>

namespace igtlio
{

//---------------------------------------------------------------------------
vtkStandardNewMacro(CircularSectionBuffer);

//---------------------------------------------------------------------------
CircularSectionBuffer::CircularSectionBuffer()
{
#if OpenIGTLink_ENABLE_VIDEOSTREAMING
  this->BufferSize = IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM;
#else
  this->BufferSize = IGTLCB_CIRC_BUFFER_SIZE;
#endif
  this->Initialization();
}

int CircularSectionBuffer::Initialization()
{
  this->Mutex = vtkMutexLock::New();
  this->Mutex->Lock();
  // Allocate Circular buffer for the new device
  this->InUseBegin = -1;
  this->InUseEnd = -1;
  this->Last  = -1;
  this->First = -1;
  for (int i = 0; i < this->BufferSize; i ++)
    {
    this->DeviceType[i] = "";
    this->DataStatus[i] = DataEmpty;
    this->Messages[i] = igtl::MessageBase::New();
    this->Messages[i]->InitPack();
    }
  this->UpdateFlag = 0;
  this->Mutex->Unlock();
  
  return 1;
}

//---------------------------------------------------------------------------
CircularSectionBuffer::~CircularSectionBuffer()
{
  this->Mutex->Delete();
}


//---------------------------------------------------------------------------
void CircularSectionBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
// Functions to push data into the circular buffer (for receiving thread)
//
//   StartPush() :     Prepare to push data
//   GetPushBuffer():  Get MessageBase buffer from the circular buffer
//   EndPush() :       Finish pushing data. The data becomes ready to
//                     be read by monitor thread.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int CircularSectionBuffer::StartPush()
{
  this->Mutex->Lock();
  this->InPush = (this->Last + 1) % this->BufferSize;
  if (this->InPush == this->InUseBegin)
    {
    this->InPush = (this->InUseEnd + 1) % this->BufferSize;
    }
  this->Mutex->Unlock();
  return this->InPush;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer CircularSectionBuffer::GetPushBuffer()
{
  return this->Messages[this->InPush];
}

//---------------------------------------------------------------------------
void CircularSectionBuffer::EndPush()
{
  this->Mutex->Lock();
  this->Last = this->InPush;
  this->DataStatus[this->InPush] = DataFilled;
  // If the Inpush location is in the section that is currently been used. jump to the end of the section.
  
  if (this->First == -1)
    {
    this->First = 0;
    }
  else if (this->InPush == this->First)
    // when the push operation has filled all the buffer locations and occuppy the first location in the buffer.
    // The first location in the buffer is then shifted to the next location.
    {
    this->First = (this->First + 1) % this->BufferSize;
    }
  else if (this->First == this->InUseEnd)
    {
    this->First = this->InPush;
    }

  if (this->PacketMode == SinglePacketMode)
    {
    this->First = this->Last;
    }
  this->UpdateFlag = 1;
  this->Mutex->Unlock();
}


//---------------------------------------------------------------------------
// Functions to pull data into the circular buffer (for monitor thread)
//
//   StartPull() :     Prepare to pull data
//   GetPullBuffer():  Get MessageBase buffer from the circular buffer
//   EndPull() :       Finish pulling data
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
int CircularSectionBuffer::StartPull()
{
  this->Mutex->Lock();
  this->InUseBegin = this->First;
  this->InUseEnd = this->Last;
  this->Mutex->Unlock();
  return this->First;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer CircularSectionBuffer::GetPullBuffer()
{
  int currentIndex = this->InUseBegin;
  this->DataStatus[currentIndex] = DataProcessed;
  if (this->InUseBegin != this->InUseEnd)
    {
    this->InUseBegin = (this->InUseBegin+1) % this->BufferSize;
    }
  else
    {
    this->UpdateFlag = 0;
    this->InUseBegin = -1;
    }
  return this->Messages[currentIndex];
}


//---------------------------------------------------------------------------
void CircularSectionBuffer::EndPull()
{
  this->Mutex->Lock();
  this->UpdateFlag = 0;
  int nextIndex = (this->InUseEnd+1) % this->BufferSize;
  if (this->DataStatus[nextIndex] == DataFilled)
    {
    this->First = nextIndex;
    }
  else if(this->DataStatus[nextIndex] == DataProcessed || this->DataStatus[nextIndex] == DataEmpty )
    {
    this->First = InUseEnd;
    }
  this->Mutex->Unlock();
}


bool CircularSectionBuffer::IsSectionBufferInProcess()
{
  if (InUseBegin>=0)
    return true;
  return false;
}

} // namespace igtlio

