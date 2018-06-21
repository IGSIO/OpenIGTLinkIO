#include "qIGTLIOCommandWidget.h"

// Qt includes
#include <QComboBox>
#include <QDateTime>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>

// IGTLIO includes
#include "igtlioCommand.h"
#include "igtlioLogic.h"

//---------------------------------------------------------------------------
qIGTLIOCommandWidget::qIGTLIOCommandWidget(QWidget* parent) : QWidget(parent)
{
  this->setupUi();
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::setupUi()
{
  QGridLayout* layout = new QGridLayout(this);


  int line=0;

  this->ConnectorSelector = new QComboBox;
  layout->addWidget(new QLabel("Connector: "), line, 0);
  layout->addWidget(this->ConnectorSelector, line, 1);
  line++;

  this->IdEdit = new QLineEdit;
  this->addCaptionedLineEdit(layout, this->IdEdit, "ID: ", line++);

  this->TimestampEdit = new QLineEdit;
  this->addCaptionedLineEdit(layout, this->TimestampEdit, "Timestamp: ", line++);

  this->NameEdit = new QLineEdit;
  this->addCaptionedLineEdit(layout, this->NameEdit, "Command name: ", line++);
  this->NameEdit->setReadOnly(false);

  this->ContentEdit = new QTextEdit;
  layout->addWidget(new QLabel("Contents: "), line, 0);
  layout->addWidget(this->ContentEdit, line, 1);
  line++;

  this->StatusEdit = new QLineEdit;
  this->addCaptionedLineEdit(layout, this->StatusEdit, "Status: ", line++);

  this->ResponseEdit = new QTextEdit;
  this->ResponseEdit->setReadOnly(true);
  layout->addWidget(new QLabel("Response: "), line, 0);
  layout->addWidget(this->ResponseEdit, line, 1);
  line++;

  this->SendCommandButton = new QPushButton("Send command");
  layout->addWidget(this->SendCommandButton, line, 1);
  connect(this->SendCommandButton, SIGNAL(clicked()), this, SLOT(onSendCommand()));
  line++;
}

//-----------------------------------------------------------------------------
void qIGTLIOCommandWidget::setLogic(igtlioLogicPointer logic)
{
  foreach(int evendId, QList<int>()
    << igtlioLogic::ConnectionAddedEvent
    << igtlioLogic::ConnectionAboutToBeRemovedEvent)
  {
    qvtkReconnect(this->Logic, logic, evendId,
      this, SLOT(onConnectionsChanged(vtkObject*, unsigned long, void*, void*)));
  }

  this->Logic = logic;
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::addCaptionedLineEdit(QGridLayout *layout, QLineEdit* edit, QString caption, int line)
{
  layout->addWidget(new QLabel(caption), line, 0);
  edit->setReadOnly(true);
  layout->addWidget(edit, line, 1);
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::setCommand(igtlioCommandPointer command)
{
  qvtkReconnect(this->Command, command, vtkCommand::ModifiedEvent, this, SLOT(onCommandModified()));
  this->Command = command;
  this->onCommandModified();
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::blockGUI(bool on)
{
  this->NameEdit->blockSignals(on);
  this->ContentEdit->blockSignals(on);
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::onCommandModified()
{
  this->blockGUI(true);

  this->TimestampEdit->setText("");
  this->IdEdit->setText("");

  if (!this->Command)
    {
    this->blockGUI(false);
    return;
    }

  QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(this->Command->GetSentTimestamp()*1000);
  this->TimestampEdit->setText(timestamp.toString("hh:mm:ss.zzz"));

  igtlioCommand* command = dynamic_cast<igtlioCommand*>(this->Command.GetPointer());

  if (!command)
    return;

  this->IdEdit->setText(QString::number(command->GetCommandId()));
  this->NameEdit->setText(QString::fromStdString(command->GetName()));
  this->ContentEdit->setText(QString::fromStdString(command->GetCommandContent()));
  this->ResponseEdit->setText(QString::fromStdString(command->GetResponseContent()));

  std::stringstream statusSS;
  statusSS << command->GetStatusAsString();
  
  if (command->GetStatus() != igtlioCommandStatus::CommandWaiting ||
      command->GetStatus() != igtlioCommandStatus::CommandUnknown )
  {
    statusSS << " - ";
    statusSS << (command->GetSuccessful() ? "Succeeded" : "Failed");
  }
  this->StatusEdit->setText(QString::fromStdString(statusSS.str()));

  this->blockGUI(false);
}

//-----------------------------------------------------------------------------
void qIGTLIOCommandWidget::onConnectionsChanged(vtkObject *vtkNotUsed(caller), unsigned long event, void *vtkNotUsed(b), void *connector)
{
  std::string currentConnector = this->ConnectorSelector->currentText().toStdString();
  this->ConnectorSelector->clear();

  if (!this->Logic)
  {
    return;
  }

  igtlioConnector* removedConnector = NULL;
  if (event == igtlioLogic::ConnectionAboutToBeRemovedEvent)
  {
    removedConnector = static_cast<igtlioConnector*>(connector);
  }

  for (int i = 0; i < this->Logic->GetNumberOfConnectors(); ++i)
  {
    igtlioConnector* currentConnector = this->Logic->GetConnector(i);
    if (currentConnector != removedConnector)
    {
      this->ConnectorSelector->addItem(QString::fromStdString(currentConnector->GetName()));
    }
  }

  int currentIndex = -1;
  if (!removedConnector || currentConnector != removedConnector->GetName())
  {
    currentIndex = this->ConnectorSelector->findText(QString::fromStdString(currentConnector));
  }
  this->ConnectorSelector->setCurrentIndex(currentIndex);
}

//---------------------------------------------------------------------------
void qIGTLIOCommandWidget::onSendCommand()
{
  int currentIndex = this->ConnectorSelector->currentIndex();
  if (currentIndex < 0 || currentIndex >= this->Logic->GetNumberOfConnectors())
  {
    return;
  }

  igtlioConnector* connector = this->Logic->GetConnector(currentIndex);
  if (!connector || !connector->IsConnected())
  {
    return;
  }

  igtlioCommandPointer command = igtlioCommandPointer::New();
  command->SetName(this->NameEdit->text().toStdString());
  command->SetCommandContent(this->ContentEdit->toPlainText().toStdString());
  this->setCommand(command);
  connector->SendCommand(this->Command);
}
