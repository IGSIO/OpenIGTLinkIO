#include "igtlioCommandMessageCodec.h"
#include <string>
#include <iostream>
#include "igtlioTestUtilities.h"

int main(int argc, char **argv)
{
    // Generate initial string with command xml
    igtlioCommandMessageCodec initialCodec(true);
    initialCodec.SetResult( true );
    initialCodec.AddParameter( "Depth", "45" );
    initialCodec.AddParameter( "Gain", "35" );
    std::string commandContent = initialCodec.GetContent();

    std::cerr << "Command content: " << std::endl << commandContent << std::endl;

    // Try decoding string
    igtlioCommandMessageCodec codec;
    codec.SetContent( commandContent );

    if( !codec.IsReply() )
    {
        std::cerr << "Command is a reply" << std::endl;
        return TEST_FAILED;
    }

    if( !codec.GetResult() )
    {
        std::cerr << "Result should be true" << std::endl;
        return TEST_FAILED;
    }

    if( codec.GetNumberOfParameters() != 2 )
    {
        std::cerr << "There should be 2 parameters" << std::endl;
        return TEST_FAILED;
    }

    if( codec.GetParameter("Depth") != "45" )
    {
        std::cerr << "Depth should be 45" << std::endl;
        return TEST_FAILED;
    }

    if( codec.GetParameter("Gain") != "35" )
    {
        std::cerr << "Gain should be 35" << std::endl;
        return TEST_FAILED;
    }

    return TEST_SUCCESS;
}
