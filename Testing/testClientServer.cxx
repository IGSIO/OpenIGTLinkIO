#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include <vtksys/SystemTools.hxx>
#include "IGTLIOFixture.h"

int main(int argc, char **argv)
{
  ClientServerFixture fixture;

  if (!fixture.ConnectClientToServer())
    return 1;

  return 0;
}
