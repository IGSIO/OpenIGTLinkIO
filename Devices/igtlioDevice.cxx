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

