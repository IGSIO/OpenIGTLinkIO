#ifndef QIGTLIOCOMMANDDEVICEWIDGET_H
#define QIGTLIOCOMMANDDEVICEWIDGET_H

#include "qIGTLIODeviceWidget.h"

class QLineEdit;
class QGridLayout;
class QComboBox;
class QTextEdit;

// CTK includes
#include <ctkVTKObject.h>

class OPENIGTLINKIO_GUI_EXPORT vtkIGTLIOCommandDeviceWidgetCreator : public vtkIGTLIODeviceWidgetCreator
{
public:
  virtual qIGTLIODeviceWidget* Create();
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOCommandDeviceWidgetCreator *New();
  vtkTypeMacro(vtkIGTLIOCommandDeviceWidgetCreator,vtkIGTLIODeviceWidgetCreator);

};
//---------------------------------------------------------------------------

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOCommandDeviceWidget : public qIGTLIODeviceWidget
{
  Q_OBJECT

public:
  qIGTLIOCommandDeviceWidget(QWidget* parent=NULL);

protected:
  QLineEdit* DeviceNameEdit;
  QLineEdit* TimestampEdit;

  QLineEdit* IdEdit;
  QComboBox* NameEdit;
  QTextEdit* ContentEdit;

  virtual void setupUi();
  virtual void onDeviceModified();

private slots:
  void AddCaptionedLineEdit(QGridLayout *layout, QLineEdit *edit, QString caption, int line);
  void onGUIModified();
  void blockGUI(bool on);
};


#endif // QIGTLIOCOMMANDDEVICEWIDGET_H
