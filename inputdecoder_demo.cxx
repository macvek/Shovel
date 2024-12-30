/*
MIT License

Copyright (c) 2024 Maciej Aleksandrowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include "console.h"
#include "inputdecoder.h"
#include "log.h"
#include "output.h"

#ifndef BUILDONWINDOWS
#include <unistd.h>
#endif

using namespace std;

int main(int argc, char** argv) {
    Console console;
    InputDecoder decoder;
    try {
        printf("Entering raw mode.. type CTRL+C to exit\n");
        console.enableRaw();

        for(;;) {
#ifdef BUILDONWINDOWS
            DWORD readCount;
            INPUT_RECORD buffer[64];
            int ret = ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), buffer, 64, &readCount);
            if (ret == TRUE) {
                ret = readCount;
            }
#else
            AChar buffer[8];
            int ret = read(STDIN_FILENO, buffer, 8);
#endif
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
