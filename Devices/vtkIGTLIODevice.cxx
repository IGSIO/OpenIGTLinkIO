/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#include "vtkIGTLIODevice.h"

// OpenIGTLink includes
#include <igtlMessageBase.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtksys/SystemTools.hxx>
#include <vtkTimerLog.h>

// STD includes
#include <string>


//---------------------------------------------------------------------------
vtkIGTLIODevice::vtkIGTLIODevice()
{
  PushOnConnect = false;
  MessageDirection = MESSAGE_DIRECTION_IN;
  QueryTimeOut = 0;
}

//---------------------------------------------------------------------------
vtkIGTLIODevice::~vtkIGTLIODevice()
{
}

//---------------------------------------------------------------------------
void vtkIGTLIODevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "DeviceType:\t" << this->GetDeviceType() << "\n";
  os << indent << "DeviceName:\t" << this->GetDeviceName() << "\n";
  os << indent << "Timestamp:\t" << std::fixed << setprecision(6) << this->GetTimestamp() << "\n";
}

//---------------------------------------------------------------------------
std::string vtkIGTLIODevice::GetDeviceName() const
{
  return HeaderData.deviceName;
}

//---------------------------------------------------------------------------
void vtkIGTLIODevice::SetDeviceName(std::string name)
{
  HeaderData.deviceName = name;
  this->Modified();
}

//---------------------------------------------------------------------------
double vtkIGTLIODevice::GetTimestamp() const
{
  return HeaderData.timestamp;
}

//---------------------------------------------------------------------------
void vtkIGTLIODevice::SetTimestamp(double val)
{
  HeaderData.timestamp = val;
  this->Modified();
}

//---------------------------------------------------------------------------
std::vector<vtkIGTLIODevice::QueryType> vtkIGTLIODevice::GetQueries() const
{
  return Queries;
}

//---------------------------------------------------------------------------
int vtkIGTLIODevice::CheckQueryExpiration()
{
  double currentTime = vtkTimerLog::GetUniversalTime();
//  if (this->QueryWaitingQueue.size() > 0)
//    {
//    for (std::list< vtkWeakPointer<vtkMRMLIGTLQueryNode> >::iterator iter = this->QueryWaitingQueue.begin();
//      iter != this->QueryWaitingQueue.end(); /* increment in the loop to allow erase */ )
//      {
//      if (iter->GetPointer()==NULL)
//        {
//        // the node has been deleted, so remove it from the list
//        iter = this->QueryWaitingQueue.erase(iter);
//        continue;
//        }
  bool expired = false;

  for (unsigned i=0; i<Queries.size(); ++i)
    {
      double timeout = this->GetQueryTimeOut();
      if ((timeout>0)
          && (currentTime-Queries[i].Query->GetTimestamp()>timeout)
          && (Queries[i].status==QUERY_STATUS_WAITING))
        {
        Queries[i].status=QUERY_STATUS_EXPIRED;
        expired = true;
        }

    }

  if (expired)
    this->InvokeEvent(ResponseEvent);

  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLIODevice::PruneCompletedQueries()
{
  std::vector<QueryType> pruned;

  for (int i=0; i<Queries.size(); ++i)
    if (Queries[i].status == QUERY_STATUS_WAITING)
      pruned.push_back(Queries[i]);

  if (pruned.size()!=Queries.size())
    this->Modified();

  Queries = pruned;
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLIODevice::CancelQuery(int index)
{
  Queries.erase(Queries.begin()+index);
  return 0;
}

//---------------------------------------------------------------------------
void vtkIGTLIODevice::SetHeader(igtl::BaseConverter::HeaderData header)
{
  HeaderData = header;
  this->Modified();
}

//---------------------------------------------------------------------------
igtl::BaseConverter::HeaderData vtkIGTLIODevice::GetHeader()
{
  return HeaderData;
}

