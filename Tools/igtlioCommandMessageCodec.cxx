#include "igtlioCommandMessageCodec.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include <sstream>


namespace igtlio
{

vtkIGTLIOCommandMessageCodec::vtkIGTLIOCommandMessageCodec() : m_isReply( false ), m_result( false )
{
}

vtkIGTLIOCommandMessageCodec::vtkIGTLIOCommandMessageCodec( bool isReply ) : m_isReply( isReply ), m_result( false )
{
}

void vtkIGTLIOCommandMessageCodec::SetResult( bool res )
{
    m_result = res;
}

void vtkIGTLIOCommandMessageCodec::SetParameter( std::string paramName, std::string value )
{
    m_parameters.push_back( std::pair<std::string,std::string>(paramName, value ) );
}

std::string vtkIGTLIOCommandMessageCodec::GetContent()
{
    std::stringstream os;
    os << "<Command>" << std::endl;
    if( m_isReply )
    {
        std::string resString = m_result ? "true" : "false";
        os << "    <Result>" << resString << "</Result>" << std::endl;
    }
    for( int i = 0; i < m_parameters.size(); ++i )
    {
        os << "    <Parameter Name=\"" << m_parameters[i].first << "\" Value=\"" << m_parameters[i].second << "\" />" << std::endl;
    }
    os << "</Command>" << std::endl;
    return os.str();
}

void vtkIGTLIOCommandMessageCodec::SetContent( std::string content )
{
    vtkXMLDataElement * root = vtkXMLUtilities::ReadElementFromString( content.c_str() );
    for( int i = 0; i < root->GetNumberOfNestedElements(); ++i )
    {
        vtkXMLDataElement * elem = root->GetNestedElement( i );
        if( std::string(elem->GetName()) == "Result" )
        {
            m_isReply = true;
            std::string result = elem->GetCharacterData();
            m_result = result == "true" ? true : false;
        }
        else if( std::string(elem->GetName()) == "Parameter" )
        {
            std::string name = elem->GetAttribute( "Name" );
            std::string value = elem->GetAttribute( "Value" );
            m_parameters.push_back( std::pair<std::string,std::string>( name, value) );
        }
    }
}

bool vtkIGTLIOCommandMessageCodec::GetResult()
{
    return m_result;
}

std::string vtkIGTLIOCommandMessageCodec::GetParameter( std::string paramName )
{
    for( int i = 0; i < m_parameters.size(); ++i )
        if( m_parameters[i].first == paramName )
            return m_parameters[i].second;
    return std::string();
}

} //namespace igtlio
