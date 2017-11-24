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

const igtl::MessageBase::MetaDataMap& Device::GetMetaData() const
{
  return this->metaInfo;
}

void Device::ClearMetaData()
{
  this->metaInfo.clear();
}

  
bool Device::SetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE encodingScheme, std::string value)
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

bool Device::GetMetaDataElement(const std::string& key, std::string& value) const
{
  IANA_ENCODING_TYPE type;
  return GetMetaDataElement(key, type, value);
}

bool Device::GetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE& encoding, std::string& value) const
{
  if (this->metaInfo.find(key) != this->metaInfo.end())
    {
    encoding = this->metaInfo.find(key)->second.first;
    value = this->metaInfo.find(key)->second.second;
    return true;
    }

  return false;
}

unsigned int Device::GetDeviceContentModifiedEvent() const
{
  return vtkCommand::ModifiedEvent;
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

