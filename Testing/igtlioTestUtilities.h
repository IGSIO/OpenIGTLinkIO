#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>

#include "igtlioImageDevice.h"
#include "igtlioCommandDevice.h"


#define GenerateErrorIf( condition, errorMessage ) if( condition ) { std::cerr << errorMessage << std::endl; return 1; }
#define TEST_FAILED 1;
#define TEST_SUCCESS 0;

vtkSmartPointer<vtkMatrix4x4> igtlioCreateTestTransform();
vtkSmartPointer<vtkImageData> igtlioCreateTestImage();

bool igtlioContains(std::vector<int> input, int value, int count);
bool igtlioCompare(vtkSmartPointer<vtkMatrix4x4> a, vtkSmartPointer<vtkMatrix4x4> b);
bool igtlioCompare(vtkSmartPointer<vtkImageData> a, vtkSmartPointer<vtkImageData> b);
bool igtlioCompare(igtlioImageDevicePointer a, igtlioImageDevicePointer b);
bool igtlioCompareID(igtlioCommandDevicePointer a, igtlioCommandDevicePointer b);
bool igtlioCompare(igtlioCommandDevicePointer a, igtlioCommandDevicePointer b);

std::string igtlioBoolToString(bool b);

#endif // TESTUTILITIES_H
