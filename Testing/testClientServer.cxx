#include <string>
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "vtkTimerLog.h"
#include <vtksys/SystemTools.hxx>
#include "IGTLIOFixture.h"
#include "igtlioTestUtilities.h"

int main(int argc, char **argv)
{
  ClientServerFixture fixture;

  if (!fixture.ConnectClientToServer())
	return TEST_FAILED;

  return TEST_SUCCESS;
}
