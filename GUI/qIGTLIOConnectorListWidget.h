#ifndef QIGTLIOCONNECTORLISTWIDGET_H
#define QIGTLIOCONNECTORLISTWIDGET_H

#include <QWidget>

// igtlio includes
#include "igtlioGUIExport.h"
class qIGTLIOConnectorModel;

class QTreeView;
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;

///
/// A Widget showing all connections, and a GUI to add/remove/modify them.
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIOConnectorListWidget : public QWidget
{
  Q_OBJECT
public:
  qIGTLIOConnectorListWidget();
  void setLogic(vtkIGTLIOLogicPointer logic);

signals:

private slots:
  void onAddConnectorButtonClicked();
  void onRemoveConnectorButtonClicked();
  void onDebugUpdateButtonClicked();
private:
  vtkIGTLIOLogicPointer Logic;
  qIGTLIOConnectorModel* ConnectorModel;
  QTreeView* ConnectorListView;
  void selectRow(int row);
};

#endif // QIGTLIOCONNECTORLISTWIDGET_H
