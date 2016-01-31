#ifndef QIGTLIOCLIENTWIDGET_H
#define QIGTLIOCLIENTWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;


class OPENIGTLINKIO_GUI_EXPORT qIGTLIOClientWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIOClientWidget();
  void setLogic(vtkIGTLIOLogicPointer logic);

signals:

public slots:
private:
  vtkIGTLIOLogicPointer Logic;

  class qIGTLIOConnectorListWidget* ConnectorListWidget;

};

#endif // QIGTLIOCLIENTWIDGET_H
