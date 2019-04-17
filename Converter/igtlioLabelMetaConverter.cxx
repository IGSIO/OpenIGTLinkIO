
// OpenIGTLInkIO includes
#include "igtlioLabelMetaConverter.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlLabelMetaMessage.h>

//---------------------------------------------------------------------------
int igtlioLabelMetaConverter::fromIGTL(igtl::MessageBase::Pointer source,
  HeaderData* header,
  ContentData* dest,
  bool checkCRC,
  igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Create a message buffer to receive  data
  igtl::LabelMetaMessage::Pointer msg;
  msg = igtl::LabelMetaMessage::New();
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

  dest->LabelMetaDataElements.clear();
  int nElements = msg->GetNumberOfLabelMetaElement();
  for (int i = 0; i < nElements; i++)
  {
    igtl::LabelMetaElement::Pointer lbMetaElement;
    msg->GetLabelMetaElement(i, lbMetaElement);

    igtlUint16 size[3];
    lbMetaElement->GetSize(size);

    LabelMetaElement element;
    element.Name = lbMetaElement->GetName();
    element.DeviceName = lbMetaElement->GetDeviceName();
    element.Label = lbMetaElement->GetLabel();
    unsigned char r,b,g,a = 0;
    lbMetaElement->GetRGBA(r,g,b,a);
    element.RGBA[0] = r;
    element.RGBA[1] = g;
    element.RGBA[2] = b;
    element.RGBA[3] = a;
    element.Size[0] = size[0];
    element.Size[1] = size[1];
    element.Size[2] = size[2];
    element.Owner = lbMetaElement->GetOwner();
    dest->LabelMetaDataElements.push_back(element);
  }

  return 1;
}

//---------------------------------------------------------------------------
int igtlioLabelMetaConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::LabelMetaMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
  {
    *dest = igtl::LabelMetaMessage::New();
  }
  (*dest)->InitPack();
  igtl::LabelMetaMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
  {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);

  for (LabelMetaDataList::const_iterator labelMetaDataIt = source.LabelMetaDataElements.begin(); labelMetaDataIt != source.LabelMetaDataElements.end(); ++labelMetaDataIt)
  {
    igtl::LabelMetaElement::Pointer labelMetaElement = igtl::LabelMetaElement::New();
    labelMetaElement->SetName(labelMetaDataIt->Name.c_str());
    labelMetaElement->SetDeviceName(labelMetaDataIt->DeviceName.c_str());
    labelMetaElement->SetLabel(labelMetaDataIt->Label);
    labelMetaElement->SetRGBA(labelMetaDataIt->RGBA[0], labelMetaDataIt->RGBA[1], labelMetaDataIt->RGBA[2], labelMetaDataIt->RGBA[3]);
    labelMetaElement->SetSize(labelMetaDataIt->Size[0], labelMetaDataIt->Size[1], labelMetaDataIt->Size[2]);
    labelMetaElement->SetOwner(labelMetaDataIt->Owner.c_str());
    msg->AddLabelMetaElement(labelMetaElement);
  }

  return 1;
}
