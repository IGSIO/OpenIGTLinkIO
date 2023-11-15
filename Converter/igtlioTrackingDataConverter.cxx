/*==========================================================================

Portions (c) Copyright 2019 Robarts Research Institute All Rights Reserved.

==========================================================================*/

// Local includes
#include "igtlioTrackingDataConverter.h"

// VTK includes
#include <vtkMatrix4x4.h>

static std::string stream_id_to_name = "STREAM_ID_TO";
static std::string stream_id_from_name = "STREAM_ID_FROM";

//---------------------------------------------------------------------------
int igtlioTrackingDataConverter::fromIGTL(igtl::MessageBase::Pointer source,
    HeaderData* header,
    ContentData* dest,
    bool checkCRC,
    igtl::MessageBase::MetaDataMap& outMetaInfo)
{

  // Process an RTS_TDATA sub-type message
  if (strncmp(source->GetDeviceType(), "RTS_", 4) == 0)
  {
    return fromIGTLResponse(source, header, dest, checkCRC, outMetaInfo);
  }

  // Create a message buffer to receive image data
  igtl::TrackingDataMessage::Pointer transMsg;
  transMsg = igtl::TrackingDataMessage::New();
  transMsg->Copy(source); // !! TODO: copy makes performance issue.

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = transMsg->Unpack(checkCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
  {
    // TODO: error handling
    return 0;
  }

  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(transMsg), header, outMetaInfo))
  {
    return 0;
  }

  // get additional transform header info
  if (!IGTLHeaderToTDATAInfo(dynamic_pointer_cast<igtl::MessageBase>(transMsg), dest))
  {
    return 0;
  }

  igtl::TrackingDataElement::Pointer elem = igtl::TrackingDataElement::New();
  for (int i = 0; i < transMsg->GetNumberOfTrackingDataElements(); ++i)
  {
    std::string transformName("");
    // Determine transform name from meta data entries
    for (auto iter = transMsg->GetMetaData().begin(); iter != transMsg->GetMetaData().end(); ++iter)
    {
      if (iter->first.find("Index") != std::string::npos)
      {
        int num;
        std::stringstream ss;
        ss << iter->second.second;
        ss >> num;
        if (num == i)
        {
          transformName = iter->first.substr(0, iter->first.find("Index"));
          break;
        }
      }
    }

    if (!transformName.empty())
    {
        dest->trackingDataElements[i].transformName = transformName;
    }

    transMsg->GetTrackingDataElement(i, elem);
    igtl::Matrix4x4 matrix;
    elem->GetMatrix(matrix);

    float tx = matrix[0][0];
    float ty = matrix[1][0];
    float tz = matrix[2][0];
    float sx = matrix[0][1];
    float sy = matrix[1][1];
    float sz = matrix[2][1];
    float nx = matrix[0][2];
    float ny = matrix[1][2];
    float nz = matrix[2][2];
    float px = matrix[0][3];
    float py = matrix[1][3];
    float pz = matrix[2][3];

    // set volume orientation
    vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
    transform->Identity();
    transform->Element[0][0] = tx;
    transform->Element[1][0] = ty;
    transform->Element[2][0] = tz;
    transform->Element[0][1] = sx;
    transform->Element[1][1] = sy;
    transform->Element[2][1] = sz;
    transform->Element[0][2] = nx;
    transform->Element[1][2] = ny;
    transform->Element[2][2] = nz;
    transform->Element[0][3] = px;
    transform->Element[1][3] = py;
    transform->Element[2][3] = pz;

    dest->trackingDataElements[i].transform = transform;
    dest->trackingDataElements[i].deviceName = elem->GetName();
    dest->trackingDataElements[i].type = elem->GetType();
  }

  return 1;

}


//---------------------------------------------------------------------------
int igtlioTrackingDataConverter::fromIGTLResponse(igtl::MessageBase::Pointer source, HeaderData *header, ContentData *dest, bool checkCRC, igtl::MessageBase::MetaDataMap& outMetaInfo)
{
  // Handler for RTS_* message
  // TODO: This could be implemented in the parent class.

  igtl::RTSTrackingDataMessage::Pointer rtsMsg;
  rtsMsg = igtl::RTSTrackingDataMessage::New();
  rtsMsg->Copy(source);

  // Deserialize the transform data
  // If CheckCRC==0, CRC check is skipped.
  int c = rtsMsg->Unpack(checkCRC);

  if (!(c & igtl::MessageHeader::UNPACK_BODY)) // if CRC check fails
  {
    // TODO: error handling
    return 0;
  }

  // get header
  if (!IGTLtoHeader(dynamic_pointer_cast<igtl::MessageBase>(rtsMsg), header, outMetaInfo))
  {
    return 0;
  }

  //
  // TODO: RTS status should be passed to dest (ContentData needs to be updated to store the RTS status)
  //

  return 1;

}


//---------------------------------------------------------------------------
int igtlioTrackingDataConverter::IGTLHeaderToTDATAInfo(igtl::MessageBase::Pointer source, ContentData* dest)
{
  for (auto iter = source->GetMetaData().begin(); iter != source->GetMetaData().end(); ++iter)
  {
    if (iter->first.find("Index") != std::string::npos)
    {
      int num;
      std::stringstream ss;
      ss << iter->second.second;
      ss >> num;
      dest->trackingDataElements[num].transformName =  iter->first.substr(0, iter->first.find("Index"));
    }
  }

  return 1;
}

//---------------------------------------------------------------------------
int igtlioTrackingDataConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::TrackingDataMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
  {
    *dest = igtl::TrackingDataMessage::New();
  }
  (*dest)->InitPack();
  igtl::TrackingDataMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
  {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);
  TDATAMetaDataToIGTL(source, &basemsg);

  if (source.trackingDataElements.size() == 0)
  {
    std::cerr << "Got no input transforms" << std::endl;
  }

  for (int i = 0; i < source.trackingDataElements.size(); ++i)
  {
    std::map<int, ContentEntry>::const_iterator entry = source.trackingDataElements.find(i);
    vtkSmartPointer<vtkMatrix4x4> matrix = entry->second.transform;
    igtl::Matrix4x4 igtlmatrix;
    igtlmatrix[0][0] = matrix->Element[0][0];
    igtlmatrix[1][0] = matrix->Element[1][0];
    igtlmatrix[2][0] = matrix->Element[2][0];
    igtlmatrix[3][0] = matrix->Element[3][0];
    igtlmatrix[0][1] = matrix->Element[0][1];
    igtlmatrix[1][1] = matrix->Element[1][1];
    igtlmatrix[2][1] = matrix->Element[2][1];
    igtlmatrix[3][1] = matrix->Element[3][1];
    igtlmatrix[0][2] = matrix->Element[0][2];
    igtlmatrix[1][2] = matrix->Element[1][2];
    igtlmatrix[2][2] = matrix->Element[2][2];
    igtlmatrix[3][2] = matrix->Element[3][2];
    igtlmatrix[0][3] = matrix->Element[0][3];
    igtlmatrix[1][3] = matrix->Element[1][3];
    igtlmatrix[2][3] = matrix->Element[2][3];
    igtlmatrix[3][3] = matrix->Element[3][3];

    igtl::TrackingDataElement::Pointer elem = igtl::TrackingDataElement::New();
    elem->SetMatrix(igtlmatrix);
    elem->SetName(entry->second.deviceName.c_str());
    elem->SetType(entry->second.type);
    msg->AddTrackingDataElement(elem);

    std::stringstream ss;
    ss << i;
    msg->SetMetaDataElement(entry->second.transformName + "Index", IANA_TYPE_US_ASCII, ss.str());
  }

  msg->Pack();

  return 1;
}

//---------------------------------------------------------------------------
int igtlioTrackingDataConverter::IGTLToVTKTDATA(const igtl::Matrix4x4& igtlTDATA, vtkSmartPointer<vtkMatrix4x4> vtkMatrix)
{
  vtkMatrix->Identity();
  vtkMatrix->Element[0][0] = igtlTDATA[0][0];
  vtkMatrix->Element[1][0] = igtlTDATA[1][0];
  vtkMatrix->Element[2][0] = igtlTDATA[2][0];
  vtkMatrix->Element[0][1] = igtlTDATA[0][1];
  vtkMatrix->Element[1][1] = igtlTDATA[1][1];
  vtkMatrix->Element[2][1] = igtlTDATA[2][1];
  vtkMatrix->Element[0][2] = igtlTDATA[0][2];
  vtkMatrix->Element[1][2] = igtlTDATA[1][2];
  vtkMatrix->Element[2][2] = igtlTDATA[2][2];
  vtkMatrix->Element[0][3] = igtlTDATA[0][3];
  vtkMatrix->Element[1][3] = igtlTDATA[1][3];
  vtkMatrix->Element[2][3] = igtlTDATA[2][3];

  return 1;
}

//----------------------------------------------------------------------------
int igtlioTrackingDataConverter::VTKToIGTLTDATA(const vtkMatrix4x4& vtkMatrix, igtl::Matrix4x4& igtlTDATA)
{
  igtlTDATA[0][0] = vtkMatrix.Element[0][0];
  igtlTDATA[1][0] = vtkMatrix.Element[1][0];
  igtlTDATA[2][0] = vtkMatrix.Element[2][0];
  igtlTDATA[0][1] = vtkMatrix.Element[0][1];
  igtlTDATA[1][1] = vtkMatrix.Element[1][1];
  igtlTDATA[2][1] = vtkMatrix.Element[2][1];
  igtlTDATA[0][2] = vtkMatrix.Element[0][2];
  igtlTDATA[1][2] = vtkMatrix.Element[1][2];
  igtlTDATA[2][2] = vtkMatrix.Element[2][2];
  igtlTDATA[0][3] = vtkMatrix.Element[0][3];
  igtlTDATA[1][3] = vtkMatrix.Element[1][3];
  igtlTDATA[2][3] = vtkMatrix.Element[2][3];

  return 1;
}

//----------------------------------------------------------------------------
int igtlioTrackingDataConverter::TDATAMetaDataToIGTL(const ContentData& source, igtl::MessageBase::Pointer* dest)
{
  return 1;
}
