#include "qIGTLIOStringDeviceWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QLineEdit>
#include <QGridLayout>

#include "vtkCommand.h"
#include <vtkObjectFactory.h>

#include "igtlioDevice.h"
#include "igtlioStringDevice.h"
#include "qIGTLIOGuiUtilities.h"

//---------------------------------------------------------------------------
qIGTLIODeviceWidget* vtkIGTLIOStringDeviceWidgetCreator::Create()
{
  return new qIGTLIOStringDeviceWidget();
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOStringDeviceWidgetCreator::GetDeviceType() const
{
 return igtlioStringConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOStringDeviceWidgetCreator);



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


qIGTLIOStringDeviceWidget::qIGTLIOStringDeviceWidget(QWidget* parent) : qIGTLIODeviceWidget(parent)
{

}

void qIGTLIOStringDeviceWidget::setupUi()
{
  QGridLayout* layout = new QGridLayout(this);

  DeviceNameEdit = new QLineEdit;
  TimestampEdit = new QLineEdit;
  EncodingEdit = new QLineEdit;
  StringEdit = new QLineEdit;
  connect(StringEdit, SIGNAL(textEdited(const QString &)), this, SLOT(onGUIModified()));

  int line=0;
  this->AddCaptionedLineEdit(layout, DeviceNameEdit,   "device: ", line++);
  this->AddCaptionedLineEdit(layout, TimestampEdit,    "timestamp: ", line++);
  this->AddCaptionedLineEdit(layout, EncodingEdit,     "encoding: ", line++);
  this->AddCaptionedLineEdit(layout, StringEdit,      "string: ", line++, false);
}

void qIGTLIOStringDeviceWidget::AddCaptionedLineEdit(QGridLayout *layout, QLineEdit* edit, QString caption, int line, bool read_only)
{
  layout->addWidget(new QLabel(caption), line, 0);
  edit->setReadOnly(read_only);
  layout->addWidget(edit, line, 1);
}

void qIGTLIOStringDeviceWidget::onGUIModified()
{
  igtlioStringDevice* device = dynamic_cast<igtlioStringDevice*>(Device.GetPointer());
  if (!device)
    return;

  igtlioStringConverter::ContentData content = device->GetContent();
  content.encoding = EncodingEdit->text().toInt();
  content.string_msg = StringEdit->text().toStdString();
  device->SetContent(content);
}

void qIGTLIOStringDeviceWidget::blockGUI(bool on)
{
  StringEdit->blockSignals(on);
}

void qIGTLIOStringDeviceWidget::onDeviceModified()
{
  this->blockGUI(true);
  DeviceNameEdit->setText("");
  TimestampEdit->setText("");
  EncodingEdit->setText("");
  StringEdit->setText("");

  QString text = (Device ? QString(convertDeviceNameToDisplay(Device->GetDeviceName()).c_str()) : "none");
  DeviceNameEdit->setText(text);

  if (!Device)
    return;

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(Device->GetTimestamp()*1000);
  TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));

  igtlioStringDevice* device = dynamic_cast<igtlioStringDevice*>(Device.GetPointer());

  if (!device)
    return;

  EncodingEdit->setText(QString::number(device->GetContent().encoding));
  StringEdit->setText(device->GetContent().string_msg.c_str());
  this->blockGUI(false);
}
