#ifdef BUILDONWINDOWS
#include "buildonwindows.h"
#else
#include <unistd.h>
#endif

#include "input.h"
#include "output.h"

void Input::waitFor() {
    if (!keyPressed.empty()) {
        return;
    }
    
#ifdef BUILDONWINDOWS
    
    DWORD readCount;
    int ret = ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), buffer, 8, &readCount);
    if (ret == TRUE) {
        ret = readCount;
    }
#else
    int ret = read(STDIN_FILENO, buffer, 8);
#endif
    if (ret <= 0) {
        Key k;
        k.type = KeyType::ERROR;
        k.value = '?';
        k.modifier = NONE;
        keyPressed.push(k);
    }
    else {
        decodeBuffer(ret);
    }
}

Key Input::getKey() {
    if (keyPressed.empty()) {
        Key k;
        k.type = KeyType::ERROR;
        k.value = '?';
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
