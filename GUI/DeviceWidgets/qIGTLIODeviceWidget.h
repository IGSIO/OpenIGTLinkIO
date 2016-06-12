#ifndef QIGTLIODEVICEWIDGET_H
#define QIGTLIODEVICEWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"

#include <vtkSmartPointer.h>
#include <vtkObject.h>

class qIGTLIODeviceWidget;
class vtkIGTLIODevice;

class OPENIGTLINKIO_GUI_EXPORT vtkIGTLIODeviceWidgetCreator : public vtkObject
{
public:
  // Create an instance of the specific device widget, with the given device_id
  virtual qIGTLIODeviceWidget* Create() = 0;
  // Return the device_type this factory creates device widgets for.
  virtual std::string GetDeviceType() const = 0;

  vtkAbstractTypeMacro(vtkIGTLIODeviceWidgetCreator,vtkObject);
};
//---------------------------------------------------------------------------

class OPENIGTLINKIO_GUI_EXPORT qIGTLIODeviceWidget : public QWidget
{
public:
  qIGTLIODeviceWidget(QWidget* parent=NULL);
  virtual void SetDevice(vtkSmartPointer<vtkIGTLIODevice> device) = 0;
};

#endif // QIGTLIODEVICEWIDGET_H
