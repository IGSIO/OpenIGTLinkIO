#include "qIGTLIOCommandDeviceWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextEdit>
#include <QComboBox>

#include "vtkCommand.h"
#include <vtkObjectFactory.h>

#include "vtkIGTLIODevice.h"
#include "igtlCommandConverter.h"
#include "vtkIGTLIOCommandDevice.h"
#include "qIGTLIOGuiUtilities.h"


//---------------------------------------------------------------------------
qIGTLIODeviceWidget* vtkIGTLIOCommandDeviceWidgetCreator::Create()
{
  return new qIGTLIOCommandDeviceWidget();
}

//---------------------------------------------------------------------------
std::string vtkIGTLIOCommandDeviceWidgetCreator::GetDeviceType() const
{
 return igtl::CommandConverter::GetIGTLTypeName();
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOCommandDeviceWidgetCreator);



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


qIGTLIOCommandDeviceWidget::qIGTLIOCommandDeviceWidget(QWidget* parent) : qIGTLIODeviceWidget(parent)
{

}

void qIGTLIOCommandDeviceWidget::setupUi()
{
  QGridLayout* layout = new QGridLayout(this);

  DeviceNameEdit = new QLineEdit;
  TimestampEdit = new QLineEdit;
  IdEdit = new QLineEdit;

  int line=0;
  this->AddCaptionedLineEdit(layout, DeviceNameEdit,   "device: ", line++);
  this->AddCaptionedLineEdit(layout, TimestampEdit,    "timestamp: ", line++);
  this->AddCaptionedLineEdit(layout, IdEdit,         "code: ", line++);

  NameEdit = new QComboBox;
  layout->addWidget(new QLabel("Command name"), line, 0);
  layout->addWidget(NameEdit, line, 1);
  line++;

  ContentEdit = new QTextEdit;
  layout->addWidget(new QLabel("Contents"), line, 0);
  layout->addWidget(ContentEdit, line, 1);
  line++;
}

void qIGTLIOCommandDeviceWidget::AddCaptionedLineEdit(QGridLayout *layout, QLineEdit* edit, QString caption, int line)
{
  layout->addWidget(new QLabel(caption), line, 0);
  edit->setReadOnly(true);
  layout->addWidget(edit, line, 1);
}

void qIGTLIOCommandDeviceWidget::onDeviceModified()
{
  DeviceNameEdit->setText("");
  TimestampEdit->setText("");
  IdEdit->setText("");

  QString text = (Device ? QString(convertDeviceNameToDisplay(Device->GetDeviceName()).c_str()) : "none");
  DeviceNameEdit->setText(text);

  if (!Device)
    return;

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(Device->GetTimestamp()*1000);
  TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));

  vtkIGTLIOCommandDevice* device = dynamic_cast<vtkIGTLIOCommandDevice*>(Device.GetPointer());

  if (!device)
    return;

  IdEdit->setText(QString::number(device->GetContent().id));

  NameEdit->clear();
  std::vector<std::string> availableCommandNames = device->GetAvailableCommandNames();
  for (unsigned i=0; i<availableCommandNames.size(); ++i)
    {
    NameEdit->addItem(availableCommandNames[i].c_str());
    }

  NameEdit->setCurrentText(device->GetContent().name.c_str());
  ContentEdit->setText(device->GetContent().content.c_str());

}

