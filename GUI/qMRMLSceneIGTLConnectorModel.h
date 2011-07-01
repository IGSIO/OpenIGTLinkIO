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

#ifndef __qMRMLSceneIGTLConnectorModel_h
#define __qMRMLSceneIGTLConnectorModel_h

#include "qMRMLSceneDisplayableModel.h"

#include "qSlicerOpenIGTLinkIFModuleExport.h"

// Logic includes
#include "Logic/vtkSlicerOpenIGTLinkIFLogic.h"

class qMRMLSceneIGTLConnectorModelPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_OpenIGTLink
class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qMRMLSceneIGTLConnectorModel : public qMRMLSceneDisplayableModel
{
  Q_OBJECT

public:
  qMRMLSceneIGTLConnectorModel(QObject *parent=0);
  virtual ~qMRMLSceneIGTLConnectorModel();

  // Register the logic
  void setLogic(vtkSlicerOpenIGTLinkIFLogic* logic);

  // Enum for the different columns
  enum Columns{
    NameColumn = 0,
    TypeColumn = 1,
    StatusColumn = 2,
    AddressColumn = 3,
    PortColumn = 4
  };
 //  VisibilityColumn = 1,
 //  LockColumn = 2,
 //  EditColumn = 3,
 //  ValueColumn = 4,
 //  TextColumn = 5
 //};

  virtual void updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column);

protected:

  virtual vtkMRMLNode* parentNode(vtkMRMLNode* node)const;

  virtual void updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item);
  
  virtual QFlags<Qt::ItemFlag> nodeFlags(vtkMRMLNode* node, int column)const;

private:
  Q_DISABLE_COPY(qMRMLSceneIGTLConnectorModel);

  vtkSlicerOpenIGTLinkIFLogic* m_Logic;

};

#endif
