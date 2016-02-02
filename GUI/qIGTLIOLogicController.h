#ifndef QIGTLIOLOGICCONTROLLER_H
#define QIGTLIOLOGICCONTROLLER_H

#include <QObject>
class QTimer;
// CTK includes
#include <ctkVTKObject.h>

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
// igtlio includes
#include "igtlioGUIExport.h"

/// Adds a timer calling vtkIGTLIOLogic regularly.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIOLogicController : public QObject
{
  Q_OBJECT
  QVTK_OBJECT

public:
  qIGTLIOLogicController();
  void setLogic(vtkIGTLIOLogicPointer logic);

signals:

public slots:
private slots:
  void onConnectionsChanged();
private slots:
  void importDataAndEvents();
private:
  QTimer* ImportDataAndEventsTimer;
  vtkIGTLIOLogicPointer Logic;

};

#endif // QIGTLIOLOGICCONTROLLER_H
