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


  int line=0;
  DeviceNameEdit = new QLineEdit;
  this->AddCaptionedLineEdit(layout, DeviceNameEdit,   "device: ", line++);

  TimestampEdit = new QLineEdit;
  this->AddCaptionedLineEdit(layout, TimestampEdit,    "timestamp: ", line++);
//  this->AddCaptionedLineEdit(layout, IdEdit,         "code: ", line++);

  layout->addWidget(new QLabel("code"), line, 0);
  IdEdit = new QLineEdit;
  connect(IdEdit, SIGNAL(editingFinished()), this, SLOT(onGUIModified()) );
  layout->addWidget(IdEdit, line, 1);
  line++;

  NameEdit = new QComboBox;
  NameEdit->setEditable(true);
  connect(NameEdit, SIGNAL(editTextChanged(QString)), this, SLOT(onGUIModified()) );
  layout->addWidget(new QLabel("Command name"), line, 0);
  layout->addWidget(NameEdit, line, 1);
  line++;

  ContentEdit = new QTextEdit;
  connect(ContentEdit, SIGNAL(textChanged()), this, SLOT(onGUIModified()) );
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

void qIGTLIOCommandDeviceWidget::onGUIModified()
{
  vtkIGTLIOCommandDevice* device = dynamic_cast<vtkIGTLIOCommandDevice*>(Device.GetPointer());
  if (!device)
    return;

  igtl::CommandConverter::ContentData content = device->GetContent();
  content.id = IdEdit->text().toInt();
  content.name = NameEdit->currentText().toStdString();
  content.content = ContentEdit->toPlainText().toStdString();
  device->SetContent(content);
}

void qIGTLIOCommandDeviceWidget::blockGUI(bool on)
{
  IdEdit->blockSignals(on);
  NameEdit->blockSignals(on);
  ContentEdit->blockSignals(on);
}

void qIGTLIOCommandDeviceWidget::onDeviceModified()
{
  this->blockGUI(true);

  DeviceNameEdit->setText("");
  TimestampEdit->setText("");
  IdEdit->setText("");

  QString text = (Device ? QString(convertDeviceNameToDisplay(Device->GetDeviceName()).c_str()) : "none");
  DeviceNameEdit->setText(text);

  if (!Device)
    {
    this->blockGUI(false);
    return;
    }

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(Device->GetTimestamp()*1000);
  TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));

  vtkIGTLIOCommandDevice* device = dynamic_cast<vtkIGTLIOCommandDevice*>(Device.GetPointer());

  if (!device)
    return;

  IdEdit->setText(QString::number(device->GetContent().id));

  NameEdit->clear();
  std::vector<std::string> availableCommandNames = device->GetAvailableCommandNames();
  std::string currentCommand = device->GetContent().name;
  int currentIndex = -1;
  for (unsigned i=0; i<availableCommandNames.size(); ++i)
    {
      if( currentCommand == availableCommandNames[i] )
          currentIndex = i;
    NameEdit->addItem(availableCommandNames[i].c_str());
    }

  NameEdit->setCurrentIndex( currentIndex );
  ContentEdit->setText(device->GetContent().content.c_str());

  this->blockGUI(false);
}

