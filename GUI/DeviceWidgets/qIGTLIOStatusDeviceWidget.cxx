#include "qIGTLIOStatusDeviceWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QLineEdit>
#include <QGridLayout>

#include "vtkCommand.h"
#include <vtkObjectFactory.h>

#include "igtlioDevice.h"
#include "igtlioStatusConverter.h"
#include "igtlioStatusDevice.h"
#include "qIGTLIOGuiUtilities.h"

//---------------------------------------------------------------------------
qIGTLIODeviceWidget* vtkIGTLIOStatusDeviceWidgetCreator::Create()
{
  return new qIGTLIOStatusDeviceWidget();
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOStatusDeviceWidgetCreator::GetDeviceType() const
{
 return igtlioStatusConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOStatusDeviceWidgetCreator);



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


qIGTLIOStatusDeviceWidget::qIGTLIOStatusDeviceWidget(QWidget* parent) : qIGTLIODeviceWidget(parent)
{

}

void qIGTLIOStatusDeviceWidget::setupUi()
{
  QGridLayout* layout = new QGridLayout(this);

  DeviceNameEdit = new QLineEdit;
  TimestampEdit = new QLineEdit;
  CodeEdit = new QLineEdit;
  SubCodeEdit = new QLineEdit;
  ErrorNameEdit = new QLineEdit;
  StatusStringEdit = new QLineEdit;

  int line=0;
  this->AddCaptionedLineEdit(layout, DeviceNameEdit,   "device: ", line++);
  this->AddCaptionedLineEdit(layout, TimestampEdit,    "timestamp: ", line++);
  this->AddCaptionedLineEdit(layout, CodeEdit,         "code: ", line++);
  this->AddCaptionedLineEdit(layout, SubCodeEdit,      "subcode: ", line++);
  this->AddCaptionedLineEdit(layout, ErrorNameEdit,    "error: ", line++);
  this->AddCaptionedLineEdit(layout, StatusStringEdit, "status: ", line++);
}

void qIGTLIOStatusDeviceWidget::AddCaptionedLineEdit(QGridLayout *layout, QLineEdit* edit, QString caption, int line)
{
  layout->addWidget(new QLabel(caption), line, 0);
  edit->setReadOnly(true);
  layout->addWidget(edit, line, 1);
}

void qIGTLIOStatusDeviceWidget::onDeviceModified()
{
  DeviceNameEdit->setText("");
  TimestampEdit->setText("");
  CodeEdit->setText("");
  SubCodeEdit->setText("");
  ErrorNameEdit->setText("");
  StatusStringEdit->setText("");

  QString text = (Device ? QString(convertDeviceNameToDisplay(Device->GetDeviceName()).c_str()) : "none");
  DeviceNameEdit->setText(text);

  if (!Device)
    return;

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(Device->GetTimestamp()*1000);
  //mTimestampEdit->setText(timestamp.toString(Qt::ISODate));
  TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));

  igtlioStatusDevice* device = dynamic_cast<igtlioStatusDevice*>(Device.GetPointer());

  if (!device)
    return;

  CodeEdit->setText("0x"+QString::number(device->GetContent().code, 16));
  SubCodeEdit->setText("0x"+QString::number(device->GetContent().subcode, 16));
  ErrorNameEdit->setText(device->GetContent().errorname.c_str());
  StatusStringEdit->setText(device->GetContent().statusstring.c_str());
}
