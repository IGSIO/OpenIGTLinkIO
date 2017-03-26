/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#include "igtlioDevice.h"

#include <vtkTimerLog.h>


namespace igtlio
{
//---------------------------------------------------------------------------
Device::Device()
{
  PushOnConnect = false;
  MessageDirection = MESSAGE_DIRECTION_IN;
  QueryTimeOut = 0;
}

//---------------------------------------------------------------------------
Device::~Device()
{
}

//---------------------------------------------------------------------------
void Device::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "DeviceType:\t" << this->GetDeviceType() << "\n";
  os << indent << "DeviceName:\t" << this->GetDeviceName() << "\n";
  os << indent << "Timestamp:\t" << std::fixed << setprecision(6) << this->GetTimestamp() << "\n";
}

//---------------------------------------------------------------------------
std::string Device::GetDeviceName() const
{
  return HeaderData.deviceName;
}

//---------------------------------------------------------------------------
std::string Device::GetDeviceType() const
{
  return NULL;
}
  
//---------------------------------------------------------------------------
void Device::SetDeviceName(std::string name)
{
  HeaderData.deviceName = name;
  this->Modified();
}

//---------------------------------------------------------------------------
double Device::GetTimestamp() const
{
  return HeaderData.timestamp;
}

//---------------------------------------------------------------------------
void Device::SetTimestamp(double val)
{
  HeaderData.timestamp = val;
  this->Modified();
}

//---------------------------------------------------------------------------
std::vector<Device::QueryType> Device::GetQueries() const
{
  return Queries;
}

//---------------------------------------------------------------------------
int Device::CheckQueryExpiration()
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
int Device::PruneCompletedQueries()
{
  std::vector<QueryType> pruned;

  for (unsigned int i=0; i<Queries.size(); ++i)
    if (Queries[i].status == QUERY_STATUS_WAITING)
      pruned.push_back(Queries[i]);

  if (pruned.size()!=Queries.size())
    this->Modified();

  Queries = pruned;
  return 0;
}

//---------------------------------------------------------------------------
int Device::CancelQuery(int index)
{
  Queries.erase(Queries.begin()+index);
  return 0;
}

//---------------------------------------------------------------------------
void Device::SetHeader(BaseConverter::HeaderData header)
{
  HeaderData = header;
  this->Modified();
}

//---------------------------------------------------------------------------
BaseConverter::HeaderData Device::GetHeader()
{
  return HeaderData;
}

} //namespace igtlio

