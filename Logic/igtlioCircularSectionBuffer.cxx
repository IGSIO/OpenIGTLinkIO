/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

// VTK includes
#include "igtlioCircularSectionBuffer.h"

#include <vtkObjectFactory.h>
#include <vtkCriticalSection.h>
#include <vtksys/SystemTools.hxx>

// OpenIGTLink includes
#include <igtlMessageBase.h>

// OpenIGTLinkIO includes
#include "igtlioUtilities.h"

// STD includes
#include <string>

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioCircularSectionBuffer);

//---------------------------------------------------------------------------
igtlioCircularSectionBuffer::igtlioCircularSectionBuffer()
{
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  this->BufferSize = IGTLCB_CIRC_BUFFER_SIZE_VIDEOSTREAM;
#else
  this->BufferSize = IGTLCB_CIRC_BUFFER_SIZE;
#endif
  this->Initialization();
}

//---------------------------------------------------------------------------
int igtlioCircularSectionBuffer::Initialization()
{
  std::lock_guard<std::mutex> lock(this->Mutex);
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

  return 1;
}

//---------------------------------------------------------------------------
igtlioCircularSectionBuffer::~igtlioCircularSectionBuffer()
{
}


//---------------------------------------------------------------------------
void igtlioCircularSectionBuffer::PrintSelf(ostream& os, vtkIndent indent)
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
int igtlioCircularSectionBuffer::StartPush()
{
  std::lock_guard<std::mutex> lock(this->Mutex);
  this->InPush = (this->Last + 1) % this->BufferSize;
  if (this->InPush == this->InUseBegin)
    {
    this->InPush = (this->InUseEnd + 1) % this->BufferSize;
    }
  return this->InPush;
}

//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioCircularSectionBuffer::GetPushBuffer()
{
  return this->Messages[this->InPush];
}

//---------------------------------------------------------------------------
void igtlioCircularSectionBuffer::EndPush()
{
  std::lock_guard<std::mutex> lock(this->Mutex);
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
int igtlioCircularSectionBuffer::StartPull()
{
  std::lock_guard<std::mutex> lock(this->Mutex);
  this->InUseBegin = this->First;
  this->InUseEnd = this->Last;
  return this->First;
}


//---------------------------------------------------------------------------
igtl::MessageBase::Pointer igtlioCircularSectionBuffer::GetPullBuffer()
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
void igtlioCircularSectionBuffer::EndPull()
{
  std::lock_guard<std::mutex> lock(this->Mutex);
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
}


//---------------------------------------------------------------------------
bool igtlioCircularSectionBuffer::IsSectionBufferInProcess()
{
  if (InUseBegin>=0)
    return true;
  return false;
}
