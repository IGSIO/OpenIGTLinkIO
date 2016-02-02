#ifndef QIGTLIOCONNECTORLISTWIDGET_H
#define QIGTLIOCONNECTORLISTWIDGET_H

#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>

// igtlio includes
#include "igtlioGUIExport.h"
class qIGTLIOConnectorModel;
class QVBoxLayout;
class qIGTLIOConnectorPropertyWidget;
class QItemSelectionModel;

class QTreeView;
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;

///
/// A Widget showing all connections, and a GUI to add/remove/modify them.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIOConnectorListWidget : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  qIGTLIOConnectorListWidget();
  void setLogic(vtkIGTLIOLogicPointer logic);

signals:

private slots:
  void onConnectionsChanged(vtkObject *caller, void *, unsigned long event, void *);
  void onCurrentConnectorChanged();
private slots:
  void onAddConnectorButtonClicked();
  void onRemoveConnectorButtonClicked();
  void onDebugUpdateButtonClicked();
private:
  vtkIGTLIOLogicPointer Logic;
  qIGTLIOConnectorModel* ConnectorModel;
  QItemSelectionModel* SelectionModel;
  QTreeView* ConnectorListView;
  qIGTLIOConnectorPropertyWidget* ConnectorPropertyWidget;
  void selectRow(int row);
  void addButtonFrame(QVBoxLayout *topLayout);
};

#endif // QIGTLIOCONNECTORLISTWIDGET_H
