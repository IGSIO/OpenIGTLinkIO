#include <string>
#include <QApplication>
#include <QMainWindow>
#include "qIGTLIOClientWidget.h"
#include "vtkIGTLIOLogic.h"
#include "qIGTLIOLogicController.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);


  vtkIGTLIOLogicPointer logic = vtkIGTLIOLogicPointer::New();
  qIGTLIOLogicController logicController;
  logicController.setLogic(logic);

  qIGTLIOClientWidget widget(logic);

  widget.setGeometry(0, 0, 859, 811);
  widget.show();
  widget.activateWindow();
  widget.raise();

  int val = app.exec();
  return val;
}
