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
