#ifndef QIGTLIODEVICESWIDGET_H
#define QIGTLIODEVICESWIDGET_H

#include <QWidget>

#include "qIGTLIOVtkConnectionMacro.h"

// igtlio includes
#include "igtlioGUIExport.h"
class qIGTLIODevicesModel;
class QVBoxLayout;
class QModelIndex;
//class qIGTLIOConnectorPropertyWidget;
class QItemSelectionModel;
class qIGTLIODeviceButtonsWidget;
class qIGTLIODevicePropertiesWidget;
class qIGTLIODeviceAddWidget;

class QTreeView;
#include <vtkSmartPointer.h>
namespace  igtlio
{
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
}

class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicesWidget : public QWidget
{
  Q_OBJECT
  IGTLIO_QVTK_OBJECT
public:
  qIGTLIODevicesWidget();
  void setLogic(igtlio::vtkIGTLIOLogicPointer logic);

signals:

private slots:
  void onConnectionsChanged(vtkObject *caller, unsigned long event, void *b, void *connector );
  void onCurrentDeviceChanged(const QModelIndex &current, const QModelIndex &previous);
private:
  igtlio::vtkIGTLIOLogicPointer Logic;
  qIGTLIODevicesModel* DevicesModel;
  QItemSelectionModel* SelectionModel;
  qIGTLIODeviceButtonsWidget* ButtonsWidget;
  qIGTLIODevicePropertiesWidget* DevicePropertiesWidget;
  QTreeView* DevicesListView;
  qIGTLIODeviceAddWidget* AddDeviceWidget;

};

#endif // QIGTLIODEVICESWIDGET_H
