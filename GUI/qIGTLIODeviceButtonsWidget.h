#ifndef QIGTLIODEVICEBUTTONSWIDGET_H
#define QIGTLIODEVICEBUTTONSWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

class qIGTLIODevicesModelNode;
class qIGTLIODevicesModel;
class QItemSelectionModel;
#include <QPointer>
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;


class OPENIGTLINKIO_GUI_EXPORT qIGTLIODeviceButtonsWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIODeviceButtonsWidget();
  void setLogic(vtkIGTLIOLogicPointer logic);
  void setModel(qIGTLIODevicesModel* model);

private slots:
  void onCurrentConnectorChanged();
  void onActionClicked();

private:
  vtkIGTLIOLogicPointer Logic;
  QPointer<qIGTLIODevicesModel> DevicesModel;
  QList<QAction*> Actions;

  qIGTLIODevicesModelNode* getSelectedNode();
};

#endif // QIGTLIODEVICEBUTTONSWIDGET_H
