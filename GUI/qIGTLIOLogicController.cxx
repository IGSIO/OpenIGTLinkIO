#include "qIGTLIOLogicController.h"
#include <QTimer>
#include <vtkIGTLIOLogic.h>

qIGTLIOLogicController::qIGTLIOLogicController()
{
  ImportDataAndEventsTimer = new QTimer(this);
  connect(ImportDataAndEventsTimer, SIGNAL(timeout()),
          this, SLOT(importDataAndEvents()));

  //TODO: start/stop when connectornodes!=0
}

void qIGTLIOLogicController::setLogic(vtkIGTLIOLogicPointer logic)
{
  this->Logic = logic;
}


//-----------------------------------------------------------------------------
void qIGTLIOLogicController::importDataAndEvents()
{
  if (Logic)
    {
    Logic->PeriodicProcess();
    }
}
