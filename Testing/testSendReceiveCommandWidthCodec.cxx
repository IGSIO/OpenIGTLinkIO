#include "IGTLIOFixture.h"
#include "igtlioSession.h"
#include "igtlioTransformDevice.h"
#include "igtlioCommandMessageCodec.h"
#include "igtlioTestUtilities.h"

int main(int argc, char **argv)
{
    //---------------------------------------------------------------------------
    // Create client-server connection
    //---------------------------------------------------------------------------
    ClientServerFixture fixture;

    if (!fixture.ConnectClientToServer())
        return 1;
    GenerateErrorIf( fixture.Client.Logic->GetNumberOfDevices() != 0, "ERROR: Client shouldn't have devices." );

    std::cout << "*** Connection done" << std::endl;

    //---------------------------------------------------------------------------
    // Create a Command message and send it
    //---------------------------------------------------------------------------
    std::string usprobe_name = "usprobe";

    igtlio::CommandMessageCodec outCodec;
    outCodec.AddParameter("Depth","45");
    outCodec.AddParameter("Gain","35");

    igtlio::CommandDevicePointer clientDevice;
    clientDevice = fixture.Client.Session->SendCommandQuery( usprobe_name, "GetDeviceParameters", outCodec.GetContent(), igtlio::ASYNCHRONOUS );

    std::cout << "*** COMMAND query sent from Client" << std::endl;

    //---------------------------------------------------------------------------
    // Wait for server to receive the command
    //---------------------------------------------------------------------------
    if (!fixture.LoopUntilEventDetected(&fixture.Server, igtlio::Logic::CommandQueryReceivedEvent))
        return 1;

    std::cout << "*** COMMAND query received by Server" << std::endl;

    //---------------------------------------------------------------------------
    // Decode the command received by the server and make sure it is the same
    // as what was sent
    //---------------------------------------------------------------------------
    igtlio::DeviceKeyType key( igtlio::CommandConverter::GetIGTLTypeName(), usprobe_name );
    igtlio::DevicePointer genReceiveDevice = fixture.Server.Connector->GetDevice( key );
    igtlio::CommandDevicePointer serverDevice = igtlio::CommandDevice::SafeDownCast( genReceiveDevice );
    igtlio::CommandConverter::ContentData content = serverDevice->GetContent();
    igtlio::CommandMessageCodec recCodec;
    recCodec.SetContent( content.content );

    GenerateErrorIf( recCodec.IsReply() != false, "Message is not a reply" );
    GenerateErrorIf( recCodec.GetResult() != false, "Message result should be false" );
    GenerateErrorIf( recCodec.GetParameter("Depth") != std::string("45"), "Depth received by server should be 45" );
    GenerateErrorIf( recCodec.GetParameter("Gain") != std::string("35"), "Gain received by server should be 35" );

    //---------------------------------------------------------------------------
    // Sent a response to the server
    //---------------------------------------------------------------------------

    igtlio::CommandMessageCodec serverResponseCodec(true);
    serverResponseCodec.SetResult( true );
    serverResponseCodec.AddParameter( "Depth", "45" );
    serverResponseCodec.AddParameter( "Gain", "35" );

    fixture.Server.Session->SendCommandResponse( usprobe_name, "GetDeviceParameters", serverResponseCodec.GetContent() );

    //---------------------------------------------------------------------------
    // Wait for client to receive response
    //---------------------------------------------------------------------------
    if (!fixture.LoopUntilEventDetected(&fixture.Client, igtlio::Logic::CommandResponseReceivedEvent))
        return 1;

    std::cout << "*** RTS_COMMAND response received by Client" << std::endl;

    //---------------------------------------------------------------------------
    // Get the response in the client
    //---------------------------------------------------------------------------
    GenerateErrorIf(clientDevice->GetQueries().empty(),"Response not properly received");

	igtlio::CommandDevice::QueryType query = clientDevice->GetQueries()[0];
    igtlio::CommandDevicePointer queryResponse = igtlio::CommandDevice::SafeDownCast(query.Response);
    igtlio::CommandMessageCodec responseCodec;
    responseCodec.SetContent( queryResponse->GetContent().content );

    GenerateErrorIf( responseCodec.IsReply() == false, "Message should be a reply" );
    GenerateErrorIf( responseCodec.GetResult() != true, "Message result should be true" );
    GenerateErrorIf( responseCodec.GetParameter("Depth") != std::string("45"), "Depth in server response should be 45" );
    GenerateErrorIf( responseCodec.GetParameter("Gain") != std::string("35"), "Gain in server response should be 35" );

    return 0;
}
