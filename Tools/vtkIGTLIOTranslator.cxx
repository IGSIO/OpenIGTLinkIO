#include "vtkIGTLIOTranslator.h"

std::string extractStringBeforeCharacter(std::string const& s, char character)
{
    std::string::size_type pos = s.find(character);
    if (pos != std::string::npos)
    {
        return s.substr(0, pos);
    }
    else
    {
        return s;
    }
}

vtkIGTLIOTranslator::vtkIGTLIOTranslator()
{
}

std::string vtkIGTLIOTranslator::GetToolNameFromDeviceName(std::string device_name)
{
    std::string retval = extractStringBeforeCharacter(device_name, '_');
    return retval;
}

std::string vtkIGTLIOTranslator::DetermineTypeBasedOnToolName(std::string tool_name)
{
  if (tool_name.find("probe") != std::string::npos) {
      return "probe";
  }
  if (tool_name.find("pointer") != std::string::npos) {
      return "tool";
  }
  return "unknown";
}

