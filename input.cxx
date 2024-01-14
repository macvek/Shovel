#ifdef BUILDONWINDOWS

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

#else
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
#endif
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
