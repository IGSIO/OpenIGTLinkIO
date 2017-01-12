#include "vtkIGTLIOObject.h"

namespace igtlio
{

vtkIGTLIOObject::vtkIGTLIOObject()
{
    this->DisableModifiedEvent = false;
    this->ModifiedEventPending = 0;
}

//---------------------------------------------------------------------------
bool vtkIGTLIOObject::GetDisableModifiedEvent()const
{
    return DisableModifiedEvent;
}

//---------------------------------------------------------------------------
void vtkIGTLIOObject::SetDisableModifiedEvent(bool onOff)
{
  DisableModifiedEvent = onOff;
}

//---------------------------------------------------------------------------
void vtkIGTLIOObject::Modified()
{
  if (this->GetDisableModifiedEvent())
    {
    ++ModifiedEventPending;
    return;
    }
  this->Superclass::Modified();
}

//---------------------------------------------------------------------------
int vtkIGTLIOObject::InvokePendingModifiedEvent ()
{
  if ( ModifiedEventPending )
    {
    int oldModifiedEventPending = ModifiedEventPending;
    ModifiedEventPending = 0;
    this->Superclass::Modified();
    return oldModifiedEventPending;
    }
  return ModifiedEventPending;
}

} // namespace igtlio

