#include "qIGTLIOGuiUtilities.h"


std::string convertDisplayToDeviceName(std::string display_name)
{
    if (display_name=="<broadcast>")
        return "";
    return display_name;
}

std::string convertDeviceNameToDisplay(std::string internal_name)
{
    if (internal_name=="")
        return "<broadcast>";
    return internal_name;
}
