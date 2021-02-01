#include "igtlioNDArrayConverter.h"
#include "igtlNDArrayMessage.h"
#include <igtl_util.h>
#include <vtkDataArray.h>

int igtlioNDArrayConverter::fromIGTL(igtl::MessageBase::Pointer source,
                                            HeaderData* header,
                                            ContentData* dest,
                                            bool checkCRC,
                                            igtl::MessageBase::MetaDataMap& outMetaInfo)
{
    igtl::NDArrayMessage::Pointer msg;
    msg = igtl::NDArrayMessage::New();
    msg->Copy(source);

    int c = msg->Unpack(checkCRC);
    

    cbcbcpa
    if ((c & igtl::MessageHeader::UNPACK_BODY == 0))
        {
        return 0;
        }

    if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, outMetaInfo))
        return 0;

    vtkSmartPointer<vtkDataArray> NDArray_msg = dest->NDArray_msg;
    NDArray_msg->Allocate(msg->GetPackSize());
    memcpy(NDArray_msg->GetVoidPointer(0), msg->GetPackPointer(), msg->GetBodySizeToRead());
    dest->NDArray_msg = NDArray_msg;
    return 1;
}

int igtlioNDArrayConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::NDArrayMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
    *dest = igtl::NDArrayMessage::New();
  (*dest)->InitPack();
  igtl::NDArrayMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
    {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);
  
  msg->Pack();
  return 1;
}