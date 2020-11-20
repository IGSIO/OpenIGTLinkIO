
// OpenIGTLinkIO incldues
#include "igtlioPointConverter.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlPointMessage.h>

//---------------------------------------------------------------------------
int igtlioPointConverter::fromIGTL(igtl::MessageBase::Pointer source,
  HeaderData* header,
  ContentData* dest,
  bool checkCRC,
  igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Create a message buffer to receive  data
  igtl::PointMessage::Pointer msg;
  msg = igtl::PointMessage::New();
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

  dest->PointElements.clear();
  int nElements = msg->GetNumberOfPointElement();
  for (int i = 0; i < nElements; i++)
  {
    igtl::PointElement::Pointer pointElement;
    msg->GetPointElement(i, pointElement);

    PointElement element;
    element.Name = pointElement->GetName();
    element.GroupName = pointElement->GetGroupName();
    unsigned char r,b,g,a = 0;
    pointElement->GetRGBA(r,g,b,a);
    element.RGBA[0] = r;
    element.RGBA[1] = g;
    element.RGBA[2] = b;
    element.RGBA[3] = a;
    float position[3] = { 0,0,0 };
    pointElement->GetPosition(position);
    for (int i = 0; i < 3; ++i)
    {
      element.Position[i] = position[i];
    }
    element.Radius = pointElement->GetRadius();
    element.Owner = pointElement->GetOwner();
    dest->PointElements.push_back(element);
  }

  return 1;
}

//---------------------------------------------------------------------------
int igtlioPointConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::PointMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
  {
    *dest = igtl::PointMessage::New();
  }
  (*dest)->InitPack();
  igtl::PointMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
  {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  if (msg->GetNumberOfPointElement() > source.PointElements.size())
  {
    // there is no API to delete a single element, so if we need to delete then we delete all
    msg->ClearPointElement();
  }
  int pointIndex = 0;
  for (PointList::const_iterator pointIt = source.PointElements.begin(); pointIt != source.PointElements.end(); ++pointIt)
  {
    igtl::PointElement::Pointer pointElement;
    if (pointIndex < msg->GetNumberOfPointElement())
    {
      msg->GetPointElement(pointIndex, pointElement);
    }
    else
    {
      pointElement = igtl::PointElement::New();
      msg->AddPointElement(pointElement);
    }
    pointElement->SetName(pointIt->Name.c_str());
    pointElement->SetGroupName(pointIt->GroupName.c_str());
    pointElement->SetRGBA(pointIt->RGBA[0], pointIt->RGBA[1], pointIt->RGBA[2], pointIt->RGBA[3]);
    pointElement->SetPosition(pointIt->Position[0], pointIt->Position[1], pointIt->Position[2]);
    pointElement->SetRadius(pointIt->Radius);
    pointElement->SetOwner(pointIt->Owner.c_str());
    pointIndex++;
  }

  msg->Pack();
  return 1;
}
