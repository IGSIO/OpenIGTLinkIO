#include "igtlioCommandMessageCodec.h"
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    // Generate initial string with command xml
    igtlio::vtkIGTLIOCommandMessageCodec initialCodec(true);
    initialCodec.SetResult( true );
    initialCodec.SetParameter( "Depth", "45" );
    initialCodec.SetParameter( "Gain", "35" );
    std::string commandContent = initialCodec.GetContent();

    std::cerr << "Command content: " << std::endl << commandContent << std::endl;

    // Try decoding string
    igtlio::vtkIGTLIOCommandMessageCodec codec;
    codec.SetContent( commandContent );

    if( !codec.IsReply() )
    {
        std::cerr << "Command is a reply" << std::endl;
        return 1;
    }

    if( !codec.GetResult() )
    {
        std::cerr << "Result should be true" << std::endl;
        return 1;
    }

    if( codec.GetNumberOfParameters() != 2 )
    {
        std::cerr << "There should be 2 parameters" << std::endl;
        return 1;
    }

    if( codec.GetParameter("Depth") != "45" )
    {
        std::cerr << "Depth should be 45" << std::endl;
        return 1;
    }

    if( codec.GetParameter("Gain") != "35" )
    {
        std::cerr << "Gain should be 35" << std::endl;
        return 1;
    }

    return 0;
}
