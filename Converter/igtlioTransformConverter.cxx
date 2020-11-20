/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlioTransformConverter.h"

#include <vtkMatrix4x4.h>

static std::string stream_id_to_name = "STREAM_ID_TO";
static std::string stream_id_from_name = "STREAM_ID_FROM";

//---------------------------------------------------------------------------
int igtlioTransformConverter::fromIGTL(igtl::MessageBase::Pointer source,
                                 HeaderData* header,
                                 ContentData* dest,
                                 bool checkCRC,
                                 igtl::MessageBase::MetaDataMap& outMetaInfo)
{
    // Create a message buffer to receive image data
    igtl::TransformMessage::Pointer transMsg;
    transMsg = igtl::TransformMessage::New();
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
    if (!IGTLHeaderToTransformInfo(dynamic_pointer_cast<igtl::MessageBase>(transMsg), dest))
    {
      return 0;
    }

    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);

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

    //std::cerr << "\n\nmatrix = " << std::endl;
    //std::cerr << tx << ", " << ty << ", " << tz << std::endl;
    //std::cerr << sx << ", " << sy << ", " << sz << std::endl;
    //std::cerr << nx << ", " << ny << ", " << nz << std::endl;
    //std::cerr << px << ", " << py << ", " << pz << std::endl;

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

    //std::cerr << "IGTL matrix = " << std::endl;
    //transform->Print(cerr);
    //std::cerr << "MRML matrix = " << std::endl;
    //transformToParent->Print(cerr);

    dest->transform = transform;
    dest->deviceName = transMsg->GetDeviceName();

    return 1;

}

//---------------------------------------------------------------------------
int igtlioTransformConverter::IGTLHeaderToTransformInfo(igtl::MessageBase::Pointer source, ContentData* dest)
{
  source->GetMetaDataElement(stream_id_to_name, dest->streamIdTo);
  source->GetMetaDataElement(stream_id_from_name, dest->streamIdFrom);

  if(dest->streamIdTo.empty())
  {
    dest->streamIdTo = "unknown";
  }
  if(dest->streamIdFrom.empty())
  {
    dest->streamIdFrom = "unknown";
  }

  return 1;
}

//---------------------------------------------------------------------------
int igtlioTransformConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::TransformMessage::Pointer* dest, igtl::MessageBase::MetaDataMap metaInfo)
{
  if (dest->IsNull())
  {
    *dest = igtl::TransformMessage::New();
  }
  (*dest)->InitPack();
  igtl::TransformMessage::Pointer msg = *dest;

  if (!metaInfo.empty())
  {
    msg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  }
  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg, metaInfo);
  TransformMetaDataToIGTL(source, &basemsg);

  if (source.transform.Get() == NULL)
  {
    std::cerr << "Got NULL input transform" << std::endl;
  }

  vtkSmartPointer<vtkMatrix4x4> matrix = source.transform;
  igtl::Matrix4x4 igtlmatrix;

  igtlmatrix[0][0]  = matrix->Element[0][0];
  igtlmatrix[1][0]  = matrix->Element[1][0];
  igtlmatrix[2][0]  = matrix->Element[2][0];
  igtlmatrix[3][0]  = matrix->Element[3][0];
  igtlmatrix[0][1]  = matrix->Element[0][1];
  igtlmatrix[1][1]  = matrix->Element[1][1];
  igtlmatrix[2][1]  = matrix->Element[2][1];
  igtlmatrix[3][1]  = matrix->Element[3][1];
  igtlmatrix[0][2]  = matrix->Element[0][2];
  igtlmatrix[1][2]  = matrix->Element[1][2];
  igtlmatrix[2][2]  = matrix->Element[2][2];
  igtlmatrix[3][2]  = matrix->Element[3][2];
  igtlmatrix[0][3]  = matrix->Element[0][3];
  igtlmatrix[1][3]  = matrix->Element[1][3];
  igtlmatrix[2][3]  = matrix->Element[2][3];
  igtlmatrix[3][3]  = matrix->Element[3][3];

  msg->SetMatrix(igtlmatrix);
  msg->SetDeviceName(source.deviceName.c_str());
  msg->Pack();

  return 1;
}

//---------------------------------------------------------------------------
int igtlioTransformConverter::IGTLToVTKTransform(const igtl::Matrix4x4& igtlTransform, vtkSmartPointer<vtkMatrix4x4> vtkMatrix)
{
  vtkMatrix->Identity();
  vtkMatrix->Element[0][0] = igtlTransform[0][0];
  vtkMatrix->Element[1][0] = igtlTransform[1][0];
  vtkMatrix->Element[2][0] = igtlTransform[2][0];
  vtkMatrix->Element[0][1] = igtlTransform[0][1];
  vtkMatrix->Element[1][1] = igtlTransform[1][1];
  vtkMatrix->Element[2][1] = igtlTransform[2][1];
  vtkMatrix->Element[0][2] = igtlTransform[0][2];
  vtkMatrix->Element[1][2] = igtlTransform[1][2];
  vtkMatrix->Element[2][2] = igtlTransform[2][2];
  vtkMatrix->Element[0][3] = igtlTransform[0][3];
  vtkMatrix->Element[1][3] = igtlTransform[1][3];
  vtkMatrix->Element[2][3] = igtlTransform[2][3];

  return 1;
}

//----------------------------------------------------------------------------
int igtlioTransformConverter::VTKToIGTLTransform(const vtkMatrix4x4& vtkMatrix, igtl::Matrix4x4& igtlTransform)
{
  igtlTransform[0][0] = vtkMatrix.Element[0][0];
  igtlTransform[1][0] = vtkMatrix.Element[1][0];
  igtlTransform[2][0] = vtkMatrix.Element[2][0];
  igtlTransform[0][1] = vtkMatrix.Element[0][1];
  igtlTransform[1][1] = vtkMatrix.Element[1][1];
  igtlTransform[2][1] = vtkMatrix.Element[2][1];
  igtlTransform[0][2] = vtkMatrix.Element[0][2];
  igtlTransform[1][2] = vtkMatrix.Element[1][2];
  igtlTransform[2][2] = vtkMatrix.Element[2][2];
  igtlTransform[0][3] = vtkMatrix.Element[0][3];
  igtlTransform[1][3] = vtkMatrix.Element[1][3];
  igtlTransform[2][3] = vtkMatrix.Element[2][3];

  return 1;
}

//----------------------------------------------------------------------------
int igtlioTransformConverter::TransformMetaDataToIGTL(const ContentData& source, igtl::MessageBase::Pointer *dest)
{
  (*dest)->SetMetaDataElement(stream_id_to_name, IANA_TYPE_US_ASCII, source.streamIdTo);
  (*dest)->SetMetaDataElement(stream_id_from_name, IANA_TYPE_US_ASCII, source.streamIdFrom);

  return 1;
}
