#ifndef IGTLIOFIXTURE_H
#define IGTLIOFIXTURE_H


#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include "igtlioImageDevice.h"
#include "igtlioCommandDevice.h"
#include "igtlioTranslator.h"

class vtkImageData;

struct LogicFixture
{
  LogicFixture();

  void startClient();
  void startServer();

  igtlio::LogicPointer Logic;
  igtlio::ConnectorPointer Connector;
  igtlio::SessionPointer Session;

  vtkSmartPointer<class vtkCallbackCommand> LogicEventCallback;
  std::vector<int> ReceivedEvents;
};

struct ClientServerFixture
{
  LogicFixture Server;
  LogicFixture Client;
  igtlio::Translator Translator;

  bool ConnectClientToServer();
  bool LoopUntilEventDetected(LogicFixture *logic, int eventId, int count=1);

public:
  vtkSmartPointer<vtkMatrix4x4> CreateTestTransform();
  vtkSmartPointer<vtkImageData> CreateTestImage();
  int CreateTestFrame(vtkImageData* image );
};


#endif // IGTLIOFIXTURE_H
