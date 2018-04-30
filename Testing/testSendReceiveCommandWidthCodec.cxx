#include "igtlioFixture.h"
#include "igtlioSession.h"
#include "igtlioTransformDevice.h"
#include "igtlioCommandMessageCodec.h"
#include "igtlioTestUtilities.h"

int main(int argc, char **argv)
{
    //---------------------------------------------------------------------------
    // Create client-server connection
    //---------------------------------------------------------------------------
    igtlioClientServerFixture fixture;

    if (!fixture.ConnectClientToServer())
        return TEST_FAILED;
    GenerateErrorIf( fixture.Client.Logic->GetNumberOfDevices() != 0, "ERROR: Client shouldn't have devices." );

    std::cout << "*** Connection done" << std::endl;

    //---------------------------------------------------------------------------
    // Create a Command message and send it
    //---------------------------------------------------------------------------
    std::string usprobe_name = "usprobe";

    igtlioCommandMessageCodec outCodec;
    outCodec.AddParameter("Depth","45");
    outCodec.AddParameter("Gain","35");

    igtlioCommandDevicePointer clientDevice;
    clientDevice = fixture.Client.Session->SendCommand( usprobe_name, "GetDeviceParameters", outCodec.GetContent(), IGTLIO_ASYNCHRONOUS );

    std::cout << "*** COMMAND query sent from Client" << std::endl;

    //---------------------------------------------------------------------------
    // Wait for server to receive the command
    //---------------------------------------------------------------------------
    if (!fixture.LoopUntilEventDetected(&fixture.Server, igtlioLogic::CommandReceivedEvent))
        return TEST_FAILED;

    std::cout << "*** COMMAND query received by Server" << std::endl;

    //---------------------------------------------------------------------------
    // Decode the command received by the server and make sure it is the same
    // as what was sent
    //---------------------------------------------------------------------------
    igtlioDeviceKeyType key( igtlioCommandConverter::GetIGTLTypeName(), usprobe_name );
    igtlioDevicePointer genReceiveDevice = fixture.Server.Connector->GetDevice( key );
    igtlioCommandDevicePointer serverDevice = igtlioCommandDevice::SafeDownCast( genReceiveDevice );
    igtlioCommandConverter::ContentData content = serverDevice->GetContent();
    igtlioCommandMessageCodec recCodec;
    recCodec.SetContent( content.content );

    GenerateErrorIf( recCodec.IsReply() != false, "Message is not a reply" );
    GenerateErrorIf( recCodec.GetResult() != false, "Message result should be false" );
    GenerateErrorIf( recCodec.GetParameter("Depth") != std::string("45"), "Depth received by server should be 45" );
    GenerateErrorIf( recCodec.GetParameter("Gain") != std::string("35"), "Gain received by server should be 35" );

    //---------------------------------------------------------------------------
    // Sent a response to the server
    //---------------------------------------------------------------------------

    igtlioCommandMessageCodec serverResponseCodec(true);
    serverResponseCodec.SetResult( true );
    serverResponseCodec.AddParameter( "Depth", "45" );
    serverResponseCodec.AddParameter( "Gain", "35" );

    fixture.Server.Session->SendCommandResponse( usprobe_name, "GetDeviceParameters", serverResponseCodec.GetContent() );

    //---------------------------------------------------------------------------
    // Wait for client to receive response
    //---------------------------------------------------------------------------
    if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlioLogic::CommandResponseReceivedEvent))
        return TEST_FAILED;

    std::cout << "*** RTS_COMMAND response received by Client" << std::endl;

    //---------------------------------------------------------------------------
    // Get the response in the client
    //---------------------------------------------------------------------------
    GenerateErrorIf(clientDevice->GetQueries().empty(),"Response not properly received");

    igtlioCommandDevice::QueryType query = clientDevice->GetQueries()[0];
    igtlioCommandDevicePointer queryResponse = igtlioCommandDevice::SafeDownCast(query.Response);
    igtlioCommandMessageCodec responseCodec;
    responseCodec.SetContent( queryResponse->GetContent().content );

    GenerateErrorIf( responseCodec.IsReply() == false, "Message should be a reply" );
    GenerateErrorIf( responseCodec.GetResult() != true, "Message result should be true" );
    GenerateErrorIf( responseCodec.GetParameter("Depth") != std::string("45"), "Depth in server response should be 45" );
    GenerateErrorIf( responseCodec.GetParameter("Gain") != std::string("35"), "Gain in server response should be 35" );

    return TEST_SUCCESS;
}
