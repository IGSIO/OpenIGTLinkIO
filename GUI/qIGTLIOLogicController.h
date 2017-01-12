#ifndef QIGTLIOLOGICCONTROLLER_H
#define QIGTLIOLOGICCONTROLLER_H

#include <QObject>
class QTimer;

#include "qIGTLIOVtkConnectionMacro.h"

#include <vtkSmartPointer.h>
namespace igtlio
{
typedef vtkSmartPointer<class Logic> LogicPointer;
}
// igtlio includes
#include "igtlioGUIExport.h"

/// Adds a timer calling vtkIGTLIOLogic regularly.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIOLogicController : public QObject
{
  Q_OBJECT
  IGTLIO_QVTK_OBJECT

public:
  qIGTLIOLogicController();
  void setLogic(igtlio::LogicPointer logic);

signals:

public slots:
private slots:
  void onConnectionsChanged();
private slots:
  void importDataAndEvents();
private:
  QTimer* ImportDataAndEventsTimer;
  igtlio::LogicPointer Logic;

};

#endif // QIGTLIOLOGICCONTROLLER_H
