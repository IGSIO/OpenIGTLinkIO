/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

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

#ifndef __qMRMLIGTLIOSortFilterProxyModel_h
#define __qMRMLIGTLIOSortFilterProxyModel_h


#include "qSlicerOpenIGTLinkIFModuleExport.h"

// qMRML includes
#include "qMRMLSortFilterProxyModel.h"

class vtkMRMLIGTLConnectorNode;
class qMRMLIGTLIOSortFilterProxyModelPrivate;

class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qMRMLIGTLIOSortFilterProxyModel : public qMRMLSortFilterProxyModel
{
  Q_OBJECT
public:
  typedef qMRMLSortFilterProxyModel Superclass;
  qMRMLIGTLIOSortFilterProxyModel(QObject *parent=0);
  virtual ~qMRMLIGTLIOSortFilterProxyModel();

  virtual vtkMRMLIGTLConnectorNode* parentConnector(const QModelIndex & index);
  virtual int parentConnectorDirection(const QModelIndex & index);

protected:
  QScopedPointer<qMRMLIGTLIOSortFilterProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLIGTLIOSortFilterProxyModel);
  Q_DISABLE_COPY(qMRMLIGTLIOSortFilterProxyModel);
};

#endif


