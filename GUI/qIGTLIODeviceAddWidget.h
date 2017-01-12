#ifndef QIGTLIODEVICEADDWIDGET_H
#define QIGTLIODEVICEADDWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

class qIGTLIODevicesModelNode;
class qIGTLIODevicesModel;
class QItemSelectionModel;
class QComboBox;
class QLineEdit;

#include <QPointer>
#include <vtkSmartPointer.h>
namespace igtlio
{
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
}

///
/// Widget for adding Devices manually.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIODeviceAddWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIODeviceAddWidget();
  void setLogic(igtlio::vtkIGTLIOLogicPointer logic);
  void setModel(qIGTLIODevicesModel* model);

private slots:
  void onCurrentConnectorChanged();
  void onAddDevice();

private:
  igtlio::vtkIGTLIOLogicPointer Logic;
  QPointer<qIGTLIODevicesModel> DevicesModel;
//  QList<QAction*> Actions;

  QComboBox* mSelectDeviceType;
  QLineEdit* mDeviceName;
  QAction* mAddDeviceAction;

  qIGTLIODevicesModelNode* getSelectedNode();
};

#endif // QIGTLIODEVICEADDWIDGET_H
