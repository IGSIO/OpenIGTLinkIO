#ifndef IGTLIOFIXTURE_H
#define IGTLIOFIXTURE_H


#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "vtkIGTLIOImageDevice.h"
#include "vtkIGTLIOCommandDevice.h"

struct LogicFixture
{
  LogicFixture();

  void startClient();
  void startServer();

  vtkSmartPointer<vtkIGTLIOImageDevice> CreateDummyImageDevice();
  vtkSmartPointer<vtkIGTLIOCommandDevice> CreateDummyCommandDevice();

  vtkIGTLIOLogicPointer Logic;
  vtkIGTLIOConnectorPointer Connector;

  vtkSmartPointer<class vtkCallbackCommand> LogicEventCallback;
  int LastReceivedEvent;
};

struct ClientServerFixture
{
  LogicFixture Server;
  LogicFixture Client;

  bool ConnectClientToServer();
  bool LoopUntilExpectedNumberOfDevicesReached(LogicFixture logic, int expectedNumberOfDevices);
  bool LoopUntilEventDetected(LogicFixture *logic, int eventId);

};


#endif // IGTLIOFIXTURE_H
