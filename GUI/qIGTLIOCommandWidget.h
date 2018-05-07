#ifndef qIGTLIOCommandWidget_H
#define qIGTLIOCommandWidget_H

#include "igtlioGUIExport.h"

// Qt includes
#include <qwidget.h>

// igtlio includes
#include "igtlioCommand.h"
#include "qIGTLIOVtkConnectionMacro.h"

class QLineEdit;
class QGridLayout;
class QComboBox;
class QTextEdit;
class QPushButton;

typedef vtkSmartPointer<class igtlioLogic> igtlioLogicPointer;
typedef vtkSmartPointer<class igtlioConnector> igtlioConnectorPointer;

//---------------------------------------------------------------------------

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOCommandWidget : public QWidget
{
  Q_OBJECT
  IGTLIO_QVTK_OBJECT

public:
  qIGTLIOCommandWidget(QWidget* parent=NULL);

  void setLogic(igtlioLogicPointer logic);

protected:

  igtlioLogicPointer Logic;

  QComboBox* ConnectorSelector;
  QLineEdit* TimestampEdit;
  QLineEdit* IdEdit;
  QLineEdit* NameEdit;
  QTextEdit* ContentEdit;
  QLineEdit* StatusEdit;
  QTextEdit* ResponseEdit;
  QPushButton* SendCommandButton;

  virtual void setupUi();

private slots:
  void addCaptionedLineEdit(QGridLayout *layout, QLineEdit *edit, QString caption, int line);
  void blockGUI(bool on);
  void onCommandModified();
  void onSendCommand();
  void onConnectionsChanged(vtkObject *caller, unsigned long event, void *b, void *connector);

public:
  void setCommand(igtlioCommandPointer command);

private:
  igtlioCommandPointer Command;
};


#endif // qIGTLIOCommandWidget_H
