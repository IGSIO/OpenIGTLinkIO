#ifndef QIGTLIOCLIENTWIDGET_H
#define QIGTLIOCLIENTWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

#include <vtkSmartPointer.h>

typedef vtkSmartPointer<class igtlioLogic> igtlioLogicPointer;

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOClientWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIOClientWidget();
  void setLogic(igtlioLogicPointer logic);

signals:

public slots:
private:
  igtlioLogicPointer Logic;

  class qIGTLIOConnectorListWidget* ConnectorListWidget;
  class qIGTLIODevicesWidget* DevicesWidget;
  class qIGTLIOCommandWidget* CommandWidget;
};

#endif // QIGTLIOCLIENTWIDGET_H
