#ifndef VTKIGTLIOTRANSLATOR_H
#define VTKIGTLIOTRANSLATOR_H

#include "vtkObject.h"

#include "igtlioToolsExport.h"

class OPENIGTLINKIO_TOOLS_EXPORT vtkIGTLIOTranslator //: public vtkObject
{
public:
	vtkIGTLIOTranslator();

	std::string GetToolFromName(std::string device_name);

};

#endif // VTKIGTLIOTRANSLATOR_H
