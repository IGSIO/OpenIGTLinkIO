#ifndef QIGTLIOSTRINGDEVICEWIDGET_H
#define QIGTLIOSTRINGDEVICEWIDGET_H


#include "qIGTLIODeviceWidget.h"

class QLineEdit;
class QGridLayout;

class OPENIGTLINKIO_GUI_EXPORT vtkIGTLIOStringDeviceWidgetCreator : public vtkIGTLIODeviceWidgetCreator
{
public:
  virtual qIGTLIODeviceWidget* Create();
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOStringDeviceWidgetCreator *New();
  vtkTypeMacro(vtkIGTLIOStringDeviceWidgetCreator,vtkIGTLIODeviceWidgetCreator);

};
//---------------------------------------------------------------------------

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOStringDeviceWidget : public qIGTLIODeviceWidget
{
  Q_OBJECT

public:
  qIGTLIOStringDeviceWidget(QWidget* parent=NULL);

protected:
  QLineEdit* DeviceNameEdit;
  QLineEdit* TimestampEdit;

  QLineEdit* EncodingEdit;
  QLineEdit* StringEdit;

  virtual void setupUi();
  virtual void onDeviceModified();

private slots:
  void AddCaptionedLineEdit(QGridLayout *layout, QLineEdit *edit, QString caption, int line, bool read_only=true);
  void onGUIModified();
  void blockGUI(bool on);
};


#endif // QIGTLIOSTRINGDEVICEWIDGET_H
