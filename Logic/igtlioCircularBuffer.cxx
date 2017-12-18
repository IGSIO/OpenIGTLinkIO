/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 ==========================================================================*/

// VTK includes
#include "igtlioCircularBuffer.h"

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
  vtkStandardNewMacro(CircularBuffer);
  
  //---------------------------------------------------------------------------
  CircularBuffer::CircularBuffer()
  {
  this->Mutex = vtkMutexLock::New();
  this->Mutex->Lock();
  // Allocate Circular buffer for the new device
  this->InUse = -1;
  this->Last  = -1;
  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i ++)
    {
    this->DeviceType[i] = "";
    this->Size[i]       = 0;
    this->Data[i]       = NULL;
    this->Messages[i] = igtl::MessageBase::New();
    this->Messages[i]->InitPack();
    }
  
  this->UpdateFlag = 0;
  this->Mutex->Unlock();
  }
  
  
  //---------------------------------------------------------------------------
  CircularBuffer::~CircularBuffer()
  {
  this->Mutex->Lock();
  this->InUse = -1;
  this->Last  = -1;
  this->Mutex->Unlock();
  
  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i ++)
    {
    if (this->Data[i] != NULL)
      {
      delete this->Data[i];
      }
    }
  this->Mutex->Delete();
  }
  
  
  //---------------------------------------------------------------------------
  void CircularBuffer::PrintSelf(ostream& os, vtkIndent indent)
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
  int CircularBuffer::StartPush()
  {
  this->Mutex->Lock();
  this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE;
  if (this->InPush == this->InUse)
    {
    this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE;
    }
  this->Mutex->Unlock();
  
  return this->InPush;
  }
  
  //---------------------------------------------------------------------------
  igtl::MessageBase::Pointer CircularBuffer::GetPushBuffer()
  {
  return this->Messages[this->InPush];
  }
  
  //---------------------------------------------------------------------------
  void CircularBuffer::EndPush()
  {
  this->Mutex->Lock();
  this->Last = this->InPush;
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
  int CircularBuffer::StartPull()
  {
  this->Mutex->Lock();
  this->InUse = this->Last;
  this->UpdateFlag = 0;
  this->Mutex->Unlock();
  return this->Last;   // return -1 if it is not available
  }
  
  
  //---------------------------------------------------------------------------
  igtl::MessageBase::Pointer CircularBuffer::GetPullBuffer()
  {
  return this->Messages[this->InUse];
  }
  
  
  //---------------------------------------------------------------------------
  void CircularBuffer::EndPull()
  {
  this->Mutex->Lock();
  this->InUse = -1;
  this->Mutex->Unlock();
  }
  
} // namespace igtlio