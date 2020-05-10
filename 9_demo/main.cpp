#include "mbed.h"
#include "mbed_rpc.h"

RpcDigitalOut redled(LED1, "myled1");
RpcDigitalOut blueled(LED3, "myled3");
Serial pc(USBTX, USBRX);
void LEDControl(Arguments *in, Reply *out);
RPCFunction rpcLED(&LEDControl, "LEDControl");
double x, y;

int main() {
    //The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class

    // receive commands, and send back the responses
    char buf[256], outbuf[256];
    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = pc.getc();
            if (recv == '\r') {
                pc.printf("\r\n");
                break;
            }
            buf[i] = pc.putc(recv);
        }

        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        pc.printf("%s\r\n", outbuf);
    }
}
// Make sure the method takes in Arguments and Reply objects.

void LEDControl (Arguments *in, Reply *out)   {
    bool success = true;

    // In this scenario, when using RPC delimit the two arguments with a space.
    x = in->getArg<double>();
    y = in->getArg<double>();

    // Have code here to call another RPC function to wake up specific led or close it.
    char buffer[200], outbuf[256];
    char strings[20];
    int led = x;
    int on = y;
    int n = sprintf(strings, "/myled%d/write %d", led, on);

    //close all led light
    RPC::call("/myled1/write 1", outbuf);
    RPC::call("/myled3/write 1", outbuf);

    strcpy(buffer, strings);
    if (success) {
        out->putData(buffer);
    } else {
        out->putData("Failed to execute LED control.");
    }

    if(on == 0 && led == 1){
        for(int i = 0; i < 10; i++){
            RPC::call("/myled1/write 0", outbuf);
            wait(0.2);
            RPC::call("/myled1/write 1", outbuf);
            wait(0.2);
        }

    }else if(on == 0 && led == 3){
        for(int i = 0; i < 10; i++){
            RPC::call("/myled3/write 0", outbuf);
            wait(0.2);
            RPC::call("/myled3/write 1", outbuf);
            wait(0.2);
        }
    }else{
        RPC::call("/myled1/write 1", outbuf);
        RPC::call("/myled3/write 1", outbuf);
    }
}