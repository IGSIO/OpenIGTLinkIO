#ifndef IGTLIOFIXTURE_H
#define IGTLIOFIXTURE_H


#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "vtkIGTLIOImageDevice.h"

struct LogicFixture
{
  LogicFixture();

  void startClient();
  void startServer();

  vtkSmartPointer<vtkIGTLIOImageDevice> CreateDummyImageDevice();

  vtkIGTLIOLogicPointer Logic;
  vtkIGTLIOConnectorPointer Connector;
};

#endif // IGTLIOFIXTURE_H
