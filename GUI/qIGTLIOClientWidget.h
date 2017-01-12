#ifndef QIGTLIOCLIENTWIDGET_H
#define QIGTLIOCLIENTWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

#include <vtkSmartPointer.h>
namespace igtlio
{
typedef vtkSmartPointer<class Logic> LogicPointer;
}


class OPENIGTLINKIO_GUI_EXPORT qIGTLIOClientWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIOClientWidget();
  void setLogic(igtlio::LogicPointer logic);

signals:

public slots:
private:
  igtlio::LogicPointer Logic;

  class qIGTLIOConnectorListWidget* ConnectorListWidget;
  class qIGTLIODevicesWidget* DevicesWidget;

};

#endif // QIGTLIOCLIENTWIDGET_H
