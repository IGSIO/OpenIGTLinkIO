/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

#include "igtlioDevice.h"

#include <vtkTimerLog.h>

//---------------------------------------------------------------------------
igtlioDevice::igtlioDevice()
{
  PushOnConnect = false;
  MessageDirection = MESSAGE_DIRECTION_IN;
}

//---------------------------------------------------------------------------
igtlioDevice::~igtlioDevice()
{
}

//---------------------------------------------------------------------------
void igtlioDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "DeviceType:\t" << this->GetDeviceType() << "\n";
  os << indent << "DeviceName:\t" << this->GetDeviceName() << "\n";
  os << indent << "Timestamp:\t" << std::fixed << setprecision(6) << this->GetTimestamp() << "\n";
}

//---------------------------------------------------------------------------
std::string igtlioDevice::GetDeviceName() const
{
  return HeaderData.deviceName;
}

//---------------------------------------------------------------------------
std::string igtlioDevice::GetDeviceType() const
{
  return NULL;
}

//---------------------------------------------------------------------------
const igtl::MessageBase::MetaDataMap& igtlioDevice::GetMetaData() const
{
  return this->metaInfo;
}

//---------------------------------------------------------------------------
void igtlioDevice::ClearMetaData()
{
  this->metaInfo.clear();
}

//---------------------------------------------------------------------------
bool igtlioDevice::SetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE encodingScheme, std::string value)
{
  igtl_metadata_header_entry entry;
  if (key.length() > std::numeric_limits<igtl_uint16>::max())
    {
    return false;
    }
  entry.key_size = static_cast<igtl_uint16>(key.length());
  entry.value_encoding = static_cast<igtlUint16>(encodingScheme);
  entry.value_size = value.length();

  metaInfo[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encodingScheme, value);
  return true;
}

//---------------------------------------------------------------------------
bool igtlioDevice::GetMetaDataElement(const std::string& key, std::string& value) const
{
  IANA_ENCODING_TYPE type;
  return GetMetaDataElement(key, type, value);
}

//---------------------------------------------------------------------------
bool igtlioDevice::GetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE& encoding, std::string& value) const
{
  if (this->metaInfo.find(key) != this->metaInfo.end())
    {
    encoding = this->metaInfo.find(key)->second.first;
    value = this->metaInfo.find(key)->second.second;
    return true;
    }

  return false;
}

//---------------------------------------------------------------------------
unsigned int igtlioDevice::GetDeviceContentModifiedEvent() const
{
  return vtkCommand::ModifiedEvent;
}

//---------------------------------------------------------------------------
void igtlioDevice::SetDeviceName(std::string name)
{
  HeaderData.deviceName = name;
  this->Modified();
}

//---------------------------------------------------------------------------
double igtlioDevice::GetTimestamp() const
{
  return HeaderData.timestamp;
}

//---------------------------------------------------------------------------
void igtlioDevice::SetTimestamp(double val)
{
  HeaderData.timestamp = val;
  this->Modified();
}


//---------------------------------------------------------------------------
void igtlioDevice::SetHeader(igtlioBaseConverter::HeaderData header)
{
  HeaderData = header;
  this->Modified();
}

//---------------------------------------------------------------------------
igtlioBaseConverter::HeaderData igtlioDevice::GetHeader()
{
  return HeaderData;
}

