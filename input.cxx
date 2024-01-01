#include <unistd.h>
#include "input.h"
#include "output.h"
#include <stdio.h>

void Input::waitFor() {
    if (!keyPressed.empty()) {
        return;
    }
    
    int ret = read(STDIN_FILENO, buffer, 8);
    if (ret <= 0) {
        Key k;
        k.type = ERROR;
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
        k.type = ERROR;
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
