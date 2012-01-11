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

#ifndef __qMRMLIGTLIOTreeView_h
#define __qMRMLIGTLIOTreeView_h

// qMRML includes
#include "qMRMLTreeView.h"

// OpenIGTLinkIF GUI includes
#include "qSlicerOpenIGTLinkIFModuleExport.h"

class qMRMLIGTLIOTreeViewPrivate;
class vtkMRMLNode;
class vtkMRMLScene;
class vtkSlicerOpenIGTLinkIFLogic;

/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qMRMLIGTLIOTreeView : public qMRMLTreeView
{
  Q_OBJECT

public:
  typedef qMRMLTreeView Superclass;
  qMRMLIGTLIOTreeView(QWidget *parent=0);
  virtual ~qMRMLIGTLIOTreeView();

//  const char* firstSelectedNode();

  // Register the logic
  void setLogic(vtkSlicerOpenIGTLinkIFLogic* logic);

//  void toggleLockForSelected();

//  void toggleVisibilityForSelected();

//  void deleteSelected();

//  void selectedAsCollection(vtkCollection* collection);

  void setSelectedNode(const char* id);

public slots:
  void setMRMLScene(vtkMRMLScene* scene);

signals:  
  void selectable(bool);
  void addable(bool);
  void removable(bool);

protected slots:
  //  void onClicked(const QModelIndex& index);
  virtual void onCurrentRowChanged(const QModelIndex& index);

protected:
  QScopedPointer<qMRMLIGTLIOTreeViewPrivate> d_ptr;
  #ifndef QT_NO_CURSOR
    void mouseMoveEvent(QMouseEvent* e);
    bool viewportEvent(QEvent* e);
  #endif
  virtual void mousePressEvent(QMouseEvent* event);

private:
  Q_DECLARE_PRIVATE(qMRMLIGTLIOTreeView);
  Q_DISABLE_COPY(qMRMLIGTLIOTreeView);

  // toggle the visibility of an OpenIGTLinkIF
//  void onVisibilityColumnClicked(vtkMRMLNode* node);

  // toggle un-/lock of an annotation
//  void onLockColumnClicked(vtkMRMLNode* node);

};

#endif
