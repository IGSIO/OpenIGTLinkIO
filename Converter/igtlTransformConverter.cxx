/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLImage.cxx $
  Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
  Version:   $Revision: 15621 $

==========================================================================*/

#include "igtlTransformConverter.h"

// VTK includes
#include <vtkMatrix4x4.h>

namespace igtlio
{

//---------------------------------------------------------------------------
int TransformConverter::fromIGTL(igtl::MessageBase::Pointer source,
                             HeaderData* header,
                             ContentData* dest,
                             bool checkCRC)
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
int TransformConverter::toIGTL(const HeaderData& header, const ContentData& source, igtl::TransformMessage::Pointer* dest)
{
  if (dest->IsNull())
    *dest = igtl::TransformMessage::New();
  igtl::TransformMessage::Pointer msg = *dest;

  igtl::MessageBase::Pointer basemsg = dynamic_pointer_cast<igtl::MessageBase>(msg);
  HeadertoIGTL(header, &basemsg);

  if (source.transform.Get()==NULL)
    std::cerr << "Got NULL input transform" << std::endl;

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

} //namespace igtlio
