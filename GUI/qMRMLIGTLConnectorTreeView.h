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

#ifndef __qMRMLIGTLConnectorTreeView_h
#define __qMRMLIGTLConnectorTreeView_h

// Qt includes
#include <QTreeView>

// CTK includes
#include <ctkPimpl.h>

// qMRML includes
#include "qMRMLTreeView.h"

#include "qSlicerOpenIGTLinkIFModuleExport.h"

// Logic includes
#include "Logic/vtkSlicerOpenIGTLinkIFLogic.h"

class qMRMLSortFilterProxyModel;
class qMRMLIGTLConnectorTreeViewPrivate;
class vtkMRMLNode;
class vtkMRMLScene;

/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qMRMLIGTLConnectorTreeView : public qMRMLTreeView
{
  Q_OBJECT
  Q_PROPERTY(QString sceneModelType READ sceneModelType WRITE setSceneModelType)
  Q_PROPERTY(bool listenNodeModifiedEvent READ listenNodeModifiedEvent WRITE setListenNodeModifiedEvent)
  Q_PROPERTY(QStringList nodeTypes READ nodeTypes WRITE setNodeTypes)

public:
  qMRMLIGTLConnectorTreeView(QWidget *parent=0);
  virtual ~qMRMLIGTLConnectorTreeView();

  const char* firstSelectedNode();

  // Register the logic
  void setLogic(vtkSlicerOpenIGTLinkIFLogic* logic);

  void toggleLockForSelected();

  void toggleVisibilityForSelected();

  void deleteSelected();

  void selectedAsCollection(vtkCollection* collection);

  void setSelectedNode(const char* id);

public slots:
  void setMRMLScene(vtkMRMLScene* scene);
  void onSelectionChanged(const QItemSelection& index,const QItemSelection& beforeIndex);

signals:
  void currentNodeChanged(vtkMRMLNode* node);

protected slots:
  void onClicked(const QModelIndex& index);

protected:
  QScopedPointer<qMRMLIGTLConnectorTreeViewPrivate> d_ptr;
  #ifndef QT_NO_CURSOR
    void mouseMoveEvent(QMouseEvent* e);
    bool viewportEvent(QEvent* e);
  #endif
  virtual void mousePressEvent(QMouseEvent* event);

private:
  Q_DECLARE_PRIVATE(qMRMLIGTLConnectorTreeView);
  Q_DISABLE_COPY(qMRMLIGTLConnectorTreeView);

  vtkSlicerOpenIGTLinkIFLogic* m_Logic;
  
  // toggle the visibility of an OpenIGTLinkIF
  void onVisibilityColumnClicked(vtkMRMLNode* node);

  // toggle un-/lock of an annotation
  void onLockColumnClicked(vtkMRMLNode* node);

};

#endif
