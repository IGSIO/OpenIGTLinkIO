#ifndef QIGTLIOCONNECTORLISTWIDGET_H
#define QIGTLIOCONNECTORLISTWIDGET_H

#include <QWidget>

#include "qIGTLIOVtkConnectionMacro.h"

// igtlio includes
#include "igtlioGUIExport.h"
class qIGTLIOConnectorModel;
class QVBoxLayout;
class qIGTLIOConnectorPropertyWidget;
class QItemSelectionModel;

class QTreeView;
#include <vtkSmartPointer.h>

typedef vtkSmartPointer<class igtlioLogic> igtlioLogicPointer;

///
/// A Widget showing all connections, and a GUI to add/remove/modify them.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIOConnectorListWidget : public QWidget
{
  Q_OBJECT
  IGTLIO_QVTK_OBJECT

public:
  qIGTLIOConnectorListWidget();
  void setLogic(igtlioLogicPointer logic);

signals:

private slots:
  void onConnectionsChanged(vtkObject* caller, unsigned long event, void * clientData,  void* connector);
  void onCurrentConnectorChanged();
private slots:
  void onAddConnectorButtonClicked();
  void onRemoveConnectorButtonClicked();
private:
  igtlioLogicPointer Logic;
  qIGTLIOConnectorModel* ConnectorModel;
  QItemSelectionModel* SelectionModel;
  QTreeView* ConnectorListView;
  qIGTLIOConnectorPropertyWidget* ConnectorPropertyWidget;
  void selectRow(int row);
  void addButtonFrame(QVBoxLayout *topLayout);
};

#endif // QIGTLIOCONNECTORLISTWIDGET_H
