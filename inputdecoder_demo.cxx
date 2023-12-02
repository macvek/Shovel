#include <stdio.h>
#include <unistd.h>

#include "console.h"
#include "inputdecoder.h"
#include "log.h"
#include "output.h"

using namespace std;

int main(int argc, char** argv) {
    Console console;
    InputDecoder decoder;
    try {
        console.enableRaw();

        for(;;) {
            AChar buffer[16];
            int ret = read(STDIN_FILENO, buffer, 8);
            if (ret <= 0) {
                return 0;
            }

            decoder.feed(buffer, ret);
            int times = 0;
            while(decoder.canLoad()) {
                ++times;
                Key k = decoder.load();
                printf("#%i Key %c, type %s , modifier %s\r\n", times, k.value, k.typeName().c_str(), k.modifierName().c_str());
                if (k.modifier == CTRL && (k.value == 'C' || k.value =='c')) {
                    return 0;
                }
            }
        }
    }
    catch (LogPanic& panic) {

    }
}
