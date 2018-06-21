#include "igtlioFixture.h"
#include "igtlioSession.h"
#include "igtlioTransformDevice.h"
#include "igtlioCommandMessageCodec.h"
#include "igtlioTestUtilities.h"
#include "igtlioCommandConverter.h"

igtlioCommandPointer command = NULL;
void testCommandReceived(vtkObject* vtkNotUsed(object), unsigned long event, void *vtkNotUsed(clientdata), void* (calldata))
{
  command = reinterpret_cast<igtlioCommand*>(calldata);
}

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

    vtkSmartPointer<vtkCallbackCommand> commandReceivedCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    commandReceivedCallback->SetCallback(testCommandReceived);
    fixture.Server.Logic->AddObserver(igtlioCommand::CommandReceivedEvent, commandReceivedCallback);

    igtlioCommandPointer sentCommand;
    sentCommand = fixture.Client.Session->SendCommand("GetDeviceParameters", outCodec.GetContent(), IGTLIO_ASYNCHRONOUS);

    std::cout << "*** COMMAND query sent from Client" << std::endl;

    //---------------------------------------------------------------------------
    // Wait for server to receive the command
    //---------------------------------------------------------------------------
    fixture.LoopUntilEventDetected(&fixture.Server, igtlioCommand::CommandReceivedEvent);
    if (!sentCommand || sentCommand->GetStatus() != igtlioCommandStatus::CommandWaiting)
    {
      std::cout << "FAILURE: Command not sent." << std::endl;
      return TEST_FAILED;
    }

    std::cout << "*** COMMAND query received by Server" << std::endl;

    //---------------------------------------------------------------------------
    // Decode the command received by the server and make sure it is the same
    // as what was sent
    //---------------------------------------------------------------------------
    igtlioCommandPointer receivedCommand = command;
    if (!receivedCommand || sentCommand->GetName() != receivedCommand->GetName())
    {
      std::cout << "FAILURE: Command names in server and client do not match." << std::endl;
      return TEST_FAILED;
    }

    igtlioCommandMessageCodec recCodec;
    recCodec.SetContent(receivedCommand->GetCommandContent());

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

    receivedCommand->SetResponseContent(serverResponseCodec.GetContent());

    fixture.Server.Session->SendCommandResponse(receivedCommand);

    //---------------------------------------------------------------------------
    // Wait for client to receive response
    //---------------------------------------------------------------------------
    fixture.LoopUntilEventDetected(&fixture.Client, igtlioCommand::CommandResponseEvent);
    if (sentCommand->GetStatus() != igtlioCommandStatus::CommandResponseReceived)
    {
      std::cout << "FAILURE: Response sent but not received." << std::endl;
      return TEST_FAILED;
    }

    std::cout << "*** RTS_COMMAND response received by Client" << std::endl;

    //---------------------------------------------------------------------------
    // Get the response in the client
    //---------------------------------------------------------------------------
    igtlioCommandMessageCodec responseCodec;
    responseCodec.SetContent( sentCommand->GetResponseContent());

    GenerateErrorIf( responseCodec.IsReply() == false, "Message should be a reply" );
    GenerateErrorIf( responseCodec.GetResult() != true, "Message result should be true" );
    GenerateErrorIf( responseCodec.GetParameter("Depth") != std::string("45"), "Depth in server response should be 45" );
    GenerateErrorIf( responseCodec.GetParameter("Gain") != std::string("35"), "Gain in server response should be 35" );

    return TEST_SUCCESS;
}
