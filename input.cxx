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

#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <unistd.h>
#endif

#include "input.h"
#include "output.h"


Input::Input() : buffer() {
#ifdef BUILDONWINDOWS
    consoleInput = GetStdHandle(STD_INPUT_HANDLE);
#endif
}


void Input::waitFor() {
    if (!keyPressed.empty()) {
        return;
    }
    
#ifdef BUILDONWINDOWS
    DWORD readCount;
    DWORD waitResult = WaitForSingleObjectEx(consoleInput, INFINITE, TRUE); // in alertable state => would trigger timer callback
    int ret;
    if (waitResult == WAIT_OBJECT_0) {
        ret = ReadConsoleInput(consoleInput, buffer, 8, &readCount);
        if (ret == TRUE) {
            ret = readCount;
        }
    }
    else {
        // wait interrupted, behave as if no input was loaded
        ret = 0;
    }
#else
    int ret = read(STDIN_FILENO, buffer, 8);
#endif
    if (ret > 0) {
        decodeBuffer(ret);
    }
}

Key Input::getKey() {
    if (keyPressed.empty()) {
        Key k;
        k.type = KeyType::EMPTY;
        k.value = '!';
        k.modifier = NONE;
        return k;
    } else {
        Key k = keyPressed.front();
        keyPressed.pop();
        return k;
    }
}

void Input::decodeBuffer(int size) {
    decoder.feed(buffer, size);
    while(decoder.canLoad()) {
        keyPressed.push(decoder.load());
    }
}
