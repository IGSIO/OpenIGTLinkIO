#ifndef QIGTLIOGENERICDEVICEWIDGET_H
#define QIGTLIOGENERICDEVICEWIDGET_H

#include "qIGTLIODeviceWidget.h"

class QLineEdit;

// CTK includes
#include <ctkVTKObject.h>

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOGenericDeviceWidget : public qIGTLIODeviceWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  qIGTLIOGenericDeviceWidget(QWidget* parent=NULL);
  virtual void SetDevice(vtkSmartPointer<vtkIGTLIODevice> device);

private:
  vtkSmartPointer<vtkIGTLIODevice> Device;

  QLineEdit* DeviceNameEdit;
  QLineEdit* TimestampEdit;

private slots:
  void onDeviceModified();
};


#endif // QIGTLIOGENERICDEVICEWIDGET_H
