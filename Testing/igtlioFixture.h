#ifndef IGTLIOFIXTURE_H
#define IGTLIOFIXTURE_H


#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include "igtlioImageDevice.h"
#include "igtlioTranslator.h"

class vtkImageData;

struct igtlioLogicFixture
{
  igtlioLogicFixture();

  void startClient();
  void startServer();

  igtlioLogicPointer Logic;
  igtlioConnectorPointer Connector;
  igtlioSessionPointer Session;

  vtkSmartPointer<class vtkCallbackCommand> LogicEventCallback;
  std::vector<int> ReceivedEvents;
};

struct igtlioClientServerFixture
{
  igtlioLogicFixture Server;
  igtlioLogicFixture Client;
  igtlioTranslator Translator;

  bool ConnectClientToServer();
  bool LoopUntilEventDetected(igtlioLogicFixture *logic, int eventId, int count=1);

public:
  vtkSmartPointer<vtkMatrix4x4> CreateTestTransform();
  vtkSmartPointer<vtkImageData> CreateTestImage();
  int CreateTestFrame(vtkImageData* image );
};


#endif // IGTLIOFIXTURE_H
