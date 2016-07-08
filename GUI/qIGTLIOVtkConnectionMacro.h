#ifndef QIGTLIOVTKCONNECTIONMACRO_H
#define QIGTLIOVTKCONNECTIONMACRO_H

#include "vtkEventQtSlotConnect.h"

#define IGTLIO_QVTK_OBJECT_RECONNECT_METHOD                                 \
void qvtkReconnect( vtkObject* old_vtk_obj, vtkObject* vtk_obj,             \
                    unsigned long vtk_event, const QObject* qt_obj,         \
                    const char* qt_slot, float priority = 0.0,              \
                    Qt::ConnectionType connectionType = Qt::AutoConnection) \
{ \
    MyQVTK.connectPtr->Disconnect( old_vtk_obj, vtk_event, qt_obj, qt_slot ); \
    MyQVTK.connectPtr->Connect( vtk_obj, vtk_event, qt_obj, qt_slot, NULL, priority, connectionType ); \
}


#define IGTLIO_QVTK_OBJECT                         \
protected:                                         \
  IGTLIO_QVTK_OBJECT_RECONNECT_METHOD              \
private:                                           \
class QVTKConnect  \
{  \
public:  \
    QVTKConnect() { connectPtr = vtkEventQtSlotConnect::New(); } \
    ~QVTKConnect() { connectPtr->Delete(); }  \
    vtkEventQtSlotConnect * connectPtr;  \
};  \
QVTKConnect MyQVTK;  \

#endif // QIGTLIOVTKCONNECTIONMACRO_H
