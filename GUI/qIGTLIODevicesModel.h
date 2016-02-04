#ifndef QIGTLIODEVICESMODEL_H
#define QIGTLIODEVICESMODEL_H

#include <QAbstractItemModel>

// CTK includes
#include <ctkVTKObject.h>

// igtlio includes
#include "igtlioGUIExport.h"

#include "vtkIGTLIOConnector.h"
#include <vtkSmartPointer.h>
#include <QSharedPointer>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
typedef QSharedPointer<class qIGTLIODevicesModelNode> qIGTLIODevicesModelNodePointer;

///
/// A model describing all IGTL devices,
/// organized by connector and direction (IN/OUT).
///
class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicesModel : public QAbstractItemModel
{
  Q_OBJECT
  QVTK_OBJECT

public:
  qIGTLIODevicesModel(QObject *parent=0);
  virtual ~qIGTLIODevicesModel();

  virtual int columnCount (const QModelIndex& parent = QModelIndex() ) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
//  virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
  virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex& index ) const;


  void resetModel();
  void setLogic(vtkIGTLIOLogicPointer logic);

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

private slots:
  void onConnectorEvent(vtkObject *caller, void *connector, unsigned long event, void *);
  void onConnectionEvent(vtkObject *caller, void *connector, unsigned long, void *);
private:
  Q_DISABLE_COPY(qIGTLIODevicesModel);

  qIGTLIODevicesModelNode* getNodeFromIndex(const QModelIndex& index) const;

  vtkIGTLIOLogicPointer Logic;
  QStringList HeaderLabels;

  mutable qIGTLIODevicesModelNodePointer RootNode;
  void ReconnectConnector(vtkIGTLIOConnector *oldConnector, vtkIGTLIOConnector *newConnector);
  qIGTLIODevicesModelNode *FindDeviceNode(vtkIGTLIODevice *device, qIGTLIODevicesModelNode *parent);
};



#endif // QIGTLIODEVICESMODEL_H
