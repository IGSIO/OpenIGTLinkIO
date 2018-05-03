/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

=========================================================================auto=*/
#ifndef IGTLIOOBJECT_H
#define IGTLIOOBJECT_H

#include "vtkObject.h"

// IGTLIO includes
#include "igtlioLogicExport.h"

///
/// Extracted from the Slicer/MRML class vtkMRMLAbstractLogic
///
class OPENIGTLINKIO_LOGIC_EXPORT vtkIGTLIOObject : public vtkObject
{
  typedef vtkObject Superclass;
public:
    int InvokePendingModifiedEvent();
    void Modified();
    void SetDisableModifiedEvent(bool onOff);
    bool GetDisableModifiedEvent() const;
protected:
    vtkIGTLIOObject();

private:
    bool DisableModifiedEvent;
    int ModifiedEventPending;
};

#endif

