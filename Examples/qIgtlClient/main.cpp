#include <string>
#include <QApplication>
#include <QMainWindow>
#include "qIGTLIOClientWidget.h"
#include "igtlioLogic.h"
#include "qIGTLIOLogicController.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);


  igtlio::LogicPointer logic = igtlio::LogicPointer::New();
  qIGTLIOLogicController logicController;
  logicController.setLogic(logic);

  qIGTLIOClientWidget widget;
  widget.setLogic(logic);

  widget.setGeometry(0, 0, 859, 811);
  widget.show();
  widget.activateWindow();
  widget.raise();

  int val = app.exec();
  return val;
}
