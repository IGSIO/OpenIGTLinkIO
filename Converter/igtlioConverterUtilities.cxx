/*==========================================================================

Portions (c) Copyright 2018 IGSIO

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   igtlio
Module:    $RCSfile: igtlioConverterUtilities.cxx,v $
Date:      $Date: 2018/07/20 4:08:00 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "igtlioConverterUtilities.h"
#include <cmath>

//---------------------------------------------------------------------------
void igtlioConverterUtilities::VTKTransformToIGTLTransform(vtkSmartPointer<vtkMatrix4x4> ijk2ras, const int imageSize[3], const double spacing[3], igtl::Matrix4x4 &matrix)
{
  if (ijk2ras)
    {
    // Transform
    float ntx = ijk2ras->Element[0][0] / (float)spacing[0];
    float nty = ijk2ras->Element[1][0] / (float)spacing[0];
    float ntz = ijk2ras->Element[2][0] / (float)spacing[0];
    float nsx = ijk2ras->Element[0][1] / (float)spacing[1];
    float nsy = ijk2ras->Element[1][1] / (float)spacing[1];
    float nsz = ijk2ras->Element[2][1] / (float)spacing[1];
    float nnx = ijk2ras->Element[0][2] / (float)spacing[2];
    float nny = ijk2ras->Element[1][2] / (float)spacing[2];
    float nnz = ijk2ras->Element[2][2] / (float)spacing[2];
    float px  = ijk2ras->Element[0][3];
    float py  = ijk2ras->Element[1][3];
    float pz  = ijk2ras->Element[2][3];
    
    // Shift the center
    // NOTE: The center of the image should be shifted due to different
    // definitions of image origin between VTK (Slicer) and OpenIGTLink;
    // OpenIGTLink image has its origin at the center, while VTK image
    // has one at the corner.
    
    float hfovi = (float)spacing[0] * (float)(imageSize[0] - 1) / 2.0;
    float hfovj = (float)spacing[1] * (float)(imageSize[1] - 1) / 2.0;
    float hfovk = (float)spacing[2] * (float)(imageSize[2] - 1) / 2.0;
    
    float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
    float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
    float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;
    
    px = px + cx;
    py = py + cy;
    pz = pz + cz;
    
    matrix[0][0] = ntx;
    matrix[1][0] = nty;
    matrix[2][0] = ntz;
    matrix[0][1] = nsx;
    matrix[1][1] = nsy;
    matrix[2][1] = nsz;
    matrix[0][2] = nnx;
    matrix[1][2] = nny;
    matrix[2][2] = nnz;
    matrix[0][3] = px;
    matrix[1][3] = py;
    matrix[2][3] = pz;
    }
  else
    {
    igtl::IdentityMatrix(matrix);
    }
}

//---------------------------------------------------------------------------
void igtlioConverterUtilities::IGTLTransformToVTKTransform(const int imageSize[3], const float spacing[3], const igtl::Matrix4x4 matrix, vtkSmartPointer<vtkMatrix4x4> ijk2ras)
{
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

  // normalize
  float psi = sqrt(tx*tx + ty*ty + tz*tz);
  float psj = sqrt(sx*sx + sy*sy + sz*sz);
  float psk = sqrt(nx*nx + ny*ny + nz*nz);
  float ntx = tx / psi;
  float nty = ty / psi;
  float ntz = tz / psi;
  float nsx = sx / psj;
  float nsy = sy / psj;
  float nsz = sz / psj;
  float nnx = nx / psk;
  float nny = ny / psk;
  float nnz = nz / psk;

  // Shift the center
  // NOTE: The center of the image should be shifted due to different
  // definitions of image origin between VTK (Slicer) and OpenIGTLink;
  // OpenIGTLink image has its origin at the center, while VTK image
  // has one at the corner.
  float hfovi = spacing[0] * psi * (imageSize[0] - 1) / 2.0;
  float hfovj = spacing[1] * psj * (imageSize[1] - 1) / 2.0;
  float hfovk = spacing[2] * psk * (imageSize[2] - 1) / 2.0;
  //float hfovk = 0;

  float cx = ntx * hfovi + nsx * hfovj + nnx * hfovk;
  float cy = nty * hfovi + nsy * hfovj + nny * hfovk;
  float cz = ntz * hfovi + nsz * hfovj + nnz * hfovk;
  px = px - cx;
  py = py - cy;
  pz = pz - cz;

  // set volume orientation
  ijk2ras->Identity();
  ijk2ras->Element[0][0] = ntx*spacing[0];
  ijk2ras->Element[1][0] = nty*spacing[0];
  ijk2ras->Element[2][0] = ntz*spacing[0];
  ijk2ras->Element[0][1] = nsx*spacing[1];
  ijk2ras->Element[1][1] = nsy*spacing[1];
  ijk2ras->Element[2][1] = nsz*spacing[1];
  ijk2ras->Element[0][2] = nnx*spacing[2];
  ijk2ras->Element[1][2] = nny*spacing[2];
  ijk2ras->Element[2][2] = nnz*spacing[2];
  ijk2ras->Element[0][3] = px;
  ijk2ras->Element[1][3] = py;
  ijk2ras->Element[2][3] = pz;
}

