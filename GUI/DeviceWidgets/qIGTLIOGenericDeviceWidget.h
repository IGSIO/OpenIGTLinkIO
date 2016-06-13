#ifndef QIGTLIOGENERICDEVICEWIDGET_H
#define QIGTLIOGENERICDEVICEWIDGET_H

#include "qIGTLIODeviceWidget.h"

class QLineEdit;


class OPENIGTLINKIO_GUI_EXPORT qIGTLIOGenericDeviceWidget : public qIGTLIODeviceWidget
{
public:
  qIGTLIOGenericDeviceWidget(QWidget* parent=NULL);

protected:
  virtual void setupUi();
  QLineEdit* DeviceNameEdit;
  QLineEdit* TimestampEdit;
  virtual void onDeviceModified();
};


#endif // QIGTLIOGENERICDEVICEWIDGET_H
