/*==========================================================================

 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 ==========================================================================*/

 // VTK includes
#include "igtlioCircularBuffer.h"

#include <vtkObjectFactory.h>
#include <vtksys/SystemTools.hxx>

// OpenIGTLink includes
#include <igtlMessageBase.h>

// OpenIGTLinkIO includes
#include "igtlioUtilities.h"

// STD includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioCircularBuffer);

//---------------------------------------------------------------------------
igtlioCircularBuffer::igtlioCircularBuffer()
{
  std::lock_guard<std::recursive_mutex> lock(this->Mutex);
  // Allocate Circular buffer for the new device
  this->InUse = -1;
  this->Last = -1;
  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i++)
  {
    this->DeviceType[i] = "";
    this->Size[i] = 0;
    this->Data[i] = NULL;
    this->Messages[i] = igtl::MessageBase::New();
    this->Messages[i]->InitPack();
  }

  this->UpdateFlag = 0;
}


//---------------------------------------------------------------------------
igtlioCircularBuffer::~igtlioCircularBuffer()
{
  {
    std::lock_guard<std::recursive_mutex> lock(this->Mutex);
    this->InUse = -1;
    this->Last = -1;
  }

  for (int i = 0; i < IGTLCB_CIRC_BUFFER_SIZE; i++)
  {
    if (this->Data[i] != NULL)
    {
      delete this->Data[i];
    }
  }
}


//---------------------------------------------------------------------------
void igtlioCircularBuffer::PrintSelf(ostream& os, vtkIndent indent)
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
int igtlioCircularBuffer::StartPush()
{
  std::lock_guard<std::recursive_mutex> lock(this->Mutex);
  this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE;
  if (this->InPush == this->InUse)
  {
    this->InPush = (this->Last + 1) % IGTLCB_CIRC_BUFFER_SIZE;
  }

  return this->InPush;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioCircularBuffer::GetPushBuffer()
{
  return this->Messages[this->InPush];
}

//---------------------------------------------------------------------------
void igtlioCircularBuffer::EndPush()
{
  std::lock_guard<std::recursive_mutex> lock(this->Mutex);
  this->Last = this->InPush;
  this->UpdateFlag = 1;
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
int igtlioCircularBuffer::StartPull()
{
  std::lock_guard<std::recursive_mutex> lock(this->Mutex);
  this->InUse = this->Last;
  this->UpdateFlag = 0;
  return this->Last;   // return -1 if it is not available
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioCircularBuffer::GetPullBuffer()
{
  return this->Messages[this->InUse];
}


//---------------------------------------------------------------------------
void igtlioCircularBuffer::EndPull()
{
  std::lock_guard<std::recursive_mutex> lock(this->Mutex);
  this->InUse = -1;
}
