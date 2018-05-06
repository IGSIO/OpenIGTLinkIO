#ifndef IGTLIOCOMMANDMESSAGECODEC_H
#define IGTLIOCOMMANDMESSAGECODEC_H

#include "igtlioToolsExport.h"
#include <vector>
#include <utility>
#include <string>

class OPENIGTLINKIO_TOOLS_EXPORT igtlioCommandMessageCodec
{

public:

  igtlioCommandMessageCodec();
  igtlioCommandMessageCodec(bool isReply);

    // Build the content of the command to send
    void SetResult( bool res );
    void AddParameter( std::string paramName, std::string value );
    void AddParameter( std::string paramName, double value );
    std::string GetContent();

    // Parse the content of a command received
    void SetContent( std::string content );
    int GetNumberOfParameters() { return m_parameters.size(); }
    std::string GetParameterName( int index ) { return m_parameters[index].first; }
    std::string GetParameterValue( int index ) { return m_parameters[index].second; }
    bool IsReply() { return m_isReply; }
    bool GetResult();
    std::string GetParameter( std::string paramName );

protected:

    typedef std::vector<std::pair<std::string,std::string> > ParamContainer;
    bool m_isReply;
    bool m_result;
    ParamContainer m_parameters;
};

#endif
