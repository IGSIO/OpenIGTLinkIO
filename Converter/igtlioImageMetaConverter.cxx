
// OpenIGTLinkIO includes
#include "igtlioImageMetaConverter.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlImageMetaMessage.h>

//---------------------------------------------------------------------------
int igtlioImageMetaConverter::fromIGTL(igtl::MessageBase::Pointer source,
  HeaderData* header,
  ContentData* dest,
  bool checkCRC,
  igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Create a message buffer to receive  data
  igtl::ImageMetaMessage::Pointer msg;
  msg = igtl::ImageMetaMessage::New();
  msg->Copy(source); // !! TODO: copy makes performance issue.

  // Deserialize the data
  // If CheckCRC==0, CRC check is skipped.
  int c = msg->Unpack(checkCRC);
  if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
  {
    // TODO: error handling
    return 0;
  }

  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, outMetaInfo))
  {
    return 0;
  }

  dest->ImageMetaDataElements.clear();
  int nElements = msg->GetNumberOfImageMetaElement();
  for (int i = 0; i < nElements; i++)
  {
    igtl::ImageMetaElement::Pointer imgMetaElement;
    msg->GetImageMetaElement(i, imgMetaElement);

    igtlUint16 size[3];
    imgMetaElement->GetSize(size);

    igtl::TimeStamp::Pointer ts;
    imgMetaElement->GetTimeStamp(ts);
    double time = ts->GetTimeStamp();

    ImageMetaElement element;
    element.Name = imgMetaElement->GetName();
    element.DeviceName = imgMetaElement->GetDeviceName();
    element.Modality = imgMetaElement->GetModality();
    element.PatientName = imgMetaElement->GetPatientName();
    element.PatientID = imgMetaElement->GetPatientID();
    element.Timestamp = time;
    element.Size[0] = size[0];
    element.Size[1] = size[1];
    element.Size[2] = size[2];
    element.ScalarType = imgMetaElement->GetScalarType();
    dest->ImageMetaDataElements.push_back(element);
  }

  return 1;
}

//---------------------------------------------------------------------------
int igtlioImageMetaConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::ImageMetaMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
  {
    *dest = igtl::ImageMetaMessage::New();
  }
  (*dest)->InitPack();
  igtl::ImageMetaMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
  {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  for (ImageMetaDataList::const_iterator imageMetaDataIt = source.ImageMetaDataElements.begin(); imageMetaDataIt != source.ImageMetaDataElements.end(); ++imageMetaDataIt)
  {
    igtl::ImageMetaElement::Pointer imageMetaElement = igtl::ImageMetaElement::New();
    imageMetaElement->SetName(imageMetaDataIt->Name.c_str());
    imageMetaElement->SetDeviceName(imageMetaDataIt->DeviceName.c_str());
    imageMetaElement->SetModality(imageMetaDataIt->Modality.c_str());
    imageMetaElement->SetPatientName(imageMetaDataIt->PatientName.c_str());
    imageMetaElement->SetPatientID(imageMetaDataIt->PatientID.c_str());
    igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
    timestamp->SetTime(imageMetaDataIt->Timestamp);
    imageMetaElement->SetTimeStamp(timestamp);
    imageMetaElement->SetSize(imageMetaDataIt->Size[0], imageMetaDataIt->Size[1], imageMetaDataIt->Size[2]);
    imageMetaElement->SetScalarType(imageMetaDataIt->ScalarType);
    msg->AddImageMetaElement(imageMetaElement);
  }

  return 1;
}
