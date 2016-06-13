#ifndef QIGTLIOSTATUSDEVICEWIDGET_H
#define QIGTLIOSTATUSDEVICEWIDGET_H


#include "qIGTLIODeviceWidget.h"

class QLineEdit;
class QGridLayout;

// CTK includes
#include <ctkVTKObject.h>

class OPENIGTLINKIO_GUI_EXPORT vtkIGTLIOStatusDeviceWidgetCreator : public vtkIGTLIODeviceWidgetCreator
{
public:
  virtual qIGTLIODeviceWidget* Create();
  virtual std::string GetDeviceType() const;

  static vtkIGTLIOStatusDeviceWidgetCreator *New();
  vtkTypeMacro(vtkIGTLIOStatusDeviceWidgetCreator,vtkIGTLIODeviceWidgetCreator);

};
//---------------------------------------------------------------------------

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOStatusDeviceWidget : public qIGTLIODeviceWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  qIGTLIOStatusDeviceWidget(QWidget* parent=NULL);
  virtual void SetDevice(vtkSmartPointer<vtkIGTLIODevice> device);

private:
  vtkSmartPointer<vtkIGTLIODevice> Device;

  QLineEdit* DeviceNameEdit;
  QLineEdit* TimestampEdit;

  QLineEdit* CodeEdit;
  QLineEdit* SubCodeEdit;
  QLineEdit* ErrorNameEdit;
  QLineEdit* StatusStringEdit;

private slots:
  void onDeviceModified();
  void AddCaptionedLineEdit(QGridLayout *layout, QLineEdit *edit, QString caption, int line);
};


#endif // QIGTLIOSTATUSDEVICEWIDGET_H
