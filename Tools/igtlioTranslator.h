#ifndef IGTLIOTRANSLATOR_H
#define IGTLIOTRANSLATOR_H

#include "vtkObject.h"

#include "igtlioToolsExport.h"

class OPENIGTLINKIO_TOOLS_EXPORT igtlioTranslator
{
public:
    igtlioTranslator();

    std::string GetToolNameFromDeviceName(std::string device_name);
    std::string DetermineTypeBasedOnToolName(std::string tool_name);

};

#endif // IGTLIOTRANSLATOR_H
