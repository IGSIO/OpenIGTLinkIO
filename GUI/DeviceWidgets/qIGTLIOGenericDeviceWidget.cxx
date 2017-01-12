#include "qIGTLIOGenericDeviceWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QLineEdit>
#include <QGridLayout>

#include "vtkCommand.h"

#include "igtlioDevice.h"
#include "qIGTLIOGuiUtilities.h"

qIGTLIOGenericDeviceWidget::qIGTLIOGenericDeviceWidget(QWidget* parent) : qIGTLIODeviceWidget(parent)
{

}

void qIGTLIOGenericDeviceWidget::setupUi()
{
  QGridLayout* layout = new QGridLayout(this);

  layout->addWidget(new QLabel("device: "), 0, 0);
  DeviceNameEdit = new QLineEdit;
  DeviceNameEdit->setReadOnly(true);
  layout->addWidget(DeviceNameEdit, 0, 1);

  layout->addWidget(new QLabel("timestamp: "), 1, 0);
  TimestampEdit = new QLineEdit;
  TimestampEdit->setReadOnly(true);
  layout->addWidget(TimestampEdit, 1, 1);
}

void qIGTLIOGenericDeviceWidget::onDeviceModified()
{
  QString text = (Device ? QString(convertDeviceNameToDisplay(Device->GetDeviceName()).c_str()) : "none");
  DeviceNameEdit->setText(text);

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(Device->GetTimestamp()*1000);
  //mTimestampEdit->setText(timestamp.toString(Qt::ISODate));
  TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));
  }
