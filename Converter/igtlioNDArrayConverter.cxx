#include "igtlioNDArrayConverter.h"
#include "igtlNDArrayMessage.h"
#include <igtl_util.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkCollection.h>

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
    if ((c & igtl::MessageHeader::UNPACK_BODY == 0))
    {
        return 0;
    }

    if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(msg), header, outMetaInfo))
        return 0;

    vtkSmartPointer<vtkCollection> collection = vtkSmartPointer<vtkCollection>::New();

    for (int i = 0; i < msg->GetArray()->GetSize()[0]; ++i) {
        vtkSmartPointer<vtkDoubleArray> NDArray_msg = vtkSmartPointer<vtkDoubleArray>::New();
        NDArray_msg->SetNumberOfTuples(msg->GetArray()->GetSize()[0]);
        memcpy(NDArray_msg->GetPointer(0), static_cast<char*>(msg->GetArray()->GetRawArray()) + (NDArray_msg->GetDataSize() * NDArray_msg->GetDataTypeSize() * i), NDArray_msg->GetDataSize() * NDArray_msg->GetDataTypeSize());
        collection->AddItem(NDArray_msg);
    }
    dest->collection = collection;
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