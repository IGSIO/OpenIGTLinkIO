#ifndef __qSlicerOpenIGTLinkIFModule_h
#define __qSlicerOpenIGTLinkIFModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerOpenIGTLinkIFModuleExport.h"

class qSlicerOpenIGTLinkIFModulePrivate;

/// \ingroup Slicer_QtModules_OpenIGTLinkIF
class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qSlicerOpenIGTLinkIFModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerOpenIGTLinkIFModule(QObject *parent=0);
  virtual ~qSlicerOpenIGTLinkIFModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  /// Help to use the module
  virtual QString helpText()const;

  /// Return acknowledgements
  virtual QString acknowledgementText()const;

  /// Return a custom icon for the module
  virtual QIcon icon()const;

  virtual QStringList categories()const;

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerOpenIGTLinkIFModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerOpenIGTLinkIFModule);
  Q_DISABLE_COPY(qSlicerOpenIGTLinkIFModule);

};

#endif
