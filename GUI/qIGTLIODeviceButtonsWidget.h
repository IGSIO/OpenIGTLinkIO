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
namespace igtlio
{
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
}


class OPENIGTLINKIO_GUI_EXPORT qIGTLIODeviceButtonsWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIODeviceButtonsWidget();
  void setLogic(igtlio::vtkIGTLIOLogicPointer logic);
  void setModel(qIGTLIODevicesModel* model);

private slots:
  void onCurrentConnectorChanged();
  void onActionClicked();

private:
  igtlio::vtkIGTLIOLogicPointer Logic;
  QPointer<qIGTLIODevicesModel> DevicesModel;
  QList<QAction*> Actions;

  qIGTLIODevicesModelNode* getSelectedNode();
};

#endif // QIGTLIODEVICEBUTTONSWIDGET_H
