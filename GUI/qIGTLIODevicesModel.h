#ifndef QIGTLIODEVICESMODEL_H
#define QIGTLIODEVICESMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

#include "qIGTLIOVtkConnectionMacro.h"

// igtlio includes
#include "igtlioGUIExport.h"

#include "igtlioConnector.h"
#include <vtkSmartPointer.h>
#include <QSharedPointer>
#include <QPointer>

namespace  igtlio
{
typedef vtkSmartPointer<class Logic> LogicPointer;
}

typedef QSharedPointer<class qIGTLIODevicesModelNode> qIGTLIODevicesModelNodePointer;
class QItemSelectionModel;

///
/// A model describing all IGTL devices,
/// organized by connector and direction (IN/OUT).
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicesModel : public QAbstractItemModel
{
  Q_OBJECT
  IGTLIO_QVTK_OBJECT

public:
  qIGTLIODevicesModel(QObject *parent=0);
  virtual ~qIGTLIODevicesModel();

  virtual int columnCount (const QModelIndex& parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
  virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex& index ) const;


  void resetModel();
  void setLogic(igtlio::LogicPointer logic);

  void setSelectionModel(QItemSelectionModel* selectionModel);
  QItemSelectionModel* selectionModel();
//  qIGTLIODevicesModelNode* selectedNode();

  enum Columns{
    NameColumn = 0,
    TypeColumn,
    StatusColumn,
    VisualizationColumn,
    PushOnConnectColumn,
    NumColumns
  };
  enum Direction{
    UNDEFINED = 0,
    INCOMING,
    OUTGOING,
  };

  qIGTLIODevicesModelNode* getNodeFromIndex(const QModelIndex& index) const;

private slots:
  void onConnectorEvent(vtkObject *caller, unsigned long event, void *, void *connector);
  void onConnectionEvent(vtkObject *caller, unsigned long, void *, void *connector);
private:
  Q_DISABLE_COPY(qIGTLIODevicesModel);


  igtlio::LogicPointer Logic;
  QStringList HeaderLabels;
  QPointer<QItemSelectionModel> SelectionModel;

  mutable qIGTLIODevicesModelNodePointer RootNode;
  void ReconnectConnector(igtlio::Connector *oldConnector, igtlio::Connector *newConnector);
  qIGTLIODevicesModelNode *FindDeviceNode(igtlio::Device *device, qIGTLIODevicesModelNode *parent);
};



#endif // QIGTLIODEVICESMODEL_H
