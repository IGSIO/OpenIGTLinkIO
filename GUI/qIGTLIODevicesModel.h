#ifndef QIGTLIODEVICESMODEL_H
#define QIGTLIODEVICESMODEL_H

#include <QAbstractItemModel>

// igtlio includes
#include "igtlioGUIExport.h"

class vtkEventQtSlotConnect;

#include "vtkIGTLIOConnector.h"
#include <vtkSmartPointer.h>
#include <QSharedPointer>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
typedef QSharedPointer<class qIGTLIODevicesModelNode> qIGTLIODevicesModelNodePointer;

class OPENIGTLINKIO_GUI_EXPORT qIGTLIODevicesModel : public QAbstractItemModel
{
  Q_OBJECT

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

protected:
private slots:
  void onConnectionEvent(vtkObject *caller, unsigned long, void *, void *);
private:
  Q_DISABLE_COPY(qIGTLIODevicesModel);

  qIGTLIODevicesModelNode* getNodeFromIndex(const QModelIndex& index) const;
//  std::vector<vtkIGTLIODevicePointer> DevicesInGroup(int group) const;
//  qIGTLIODevicesModelNode* GetNode(vtkIGTLIOConnector* connector=NULL, vtkIGTLIODevice::MESSAGE_DIRECTION group=vtkIGTLIODevice::NUM_MESSAGE_DIRECTION, vtkIGTLIODevice* device=NULL) const;

  vtkIGTLIOLogicPointer Logic;
  QStringList HeaderLabels;
  vtkSmartPointer<vtkEventQtSlotConnect> Connections;

  mutable qIGTLIODevicesModelNodePointer RootNode;
//  mutable std::set<qIGTLIODevicesModelNodePointer> Nodes;
};



#endif // QIGTLIODEVICESMODEL_H
