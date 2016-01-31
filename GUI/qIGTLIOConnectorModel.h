/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qIGTLIOConnectorModel_h
#define __qIGTLIOConnectorModel_h

#include <QAbstractItemModel>

// igtlio includes
#include "igtlioGUIExport.h"

//// qMRMLWidgets includes
//#include "../Logic/vtkIGTLIOLogic.h"
//#include "qMRMLSceneDisplayableModel.h"
class vtkEventQtSlotConnect;
// Logic includes

//class qIGTLIOConnectorModelPrivate;
//class vtkMRMLNode;
#include "vtkIGTLIOConnector.h"
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;

class OPENIGTLINKIO_GUI_EXPORT qIGTLIOConnectorModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  qIGTLIOConnectorModel(QObject *parent=0);
  virtual ~qIGTLIOConnectorModel();

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
    HostnameColumn,
    PortColumn
  };

//  virtual void updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column);

//  /// As we reimplement insertNode, we need don't want to hide the other functions.
//  using qMRMLSceneModel::insertNode;
//  /// Reimplemented to listen to the vtkMRMLIGTLConnectorNode events for
//  /// connector state changes.
//  virtual QStandardItem* insertNode(vtkMRMLNode* node, QStandardItem* parent, int row);

protected:

//  virtual vtkMRMLNode* parentNode(vtkMRMLNode* node)const;

//  virtual void updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item);

//  virtual QFlags<Qt::ItemFlag> nodeFlags(vtkMRMLNode* node, int column)const;

private slots:
  void onConnectionEvent(vtkObject *caller, unsigned long, void *, void *);
private:
  Q_DISABLE_COPY(qIGTLIOConnectorModel);

  vtkIGTLIOConnector* getNodeFromIndex(const QModelIndex& index) const;

  vtkIGTLIOLogicPointer Logic;
  QStringList HeaderLabels;
  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
};

#endif
