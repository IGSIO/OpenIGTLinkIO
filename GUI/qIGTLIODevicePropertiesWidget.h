#ifndef QIGTLIODEVICEPROPERTIESWIDGET_H
#define QIGTLIODEVICEPROPERTIESWIDGET_H

#include <QWidget>
#include <vtkSmartPointer.h>

// igtlio includes
#include "igtlioGUIExport.h"

class QVBoxLayout;
class qIGTLIODevicesModelNode;
class WidgetInGroupBoxClass;
typedef vtkSmartPointer<class vtkIGTLIODeviceWidgetFactory> vtkIGTLIODeviceWidgetFactoryPointer;

class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIODevicePropertiesWidget(QWidget* parent=NULL);
  void SetNode(qIGTLIODevicesModelNode* node);

private:
  // QVBoxLayout* mLayout;
  qIGTLIODevicesModelNode* Node;
  WidgetInGroupBoxClass* WidgetInGroupBox;
  vtkIGTLIODeviceWidgetFactoryPointer DeviceWidgetFactory;
};

#endif // QIGTLIODEVICEPROPERTIESWIDGET_H
