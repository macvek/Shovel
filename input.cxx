#include <unistd.h>
#include "input.h"
#include "output.h"

void Input::waitFor() {
    int ret = read(STDIN_FILENO, buffer, 8);
    if (ret <= 0) {
        key = KEY_FAILED;
    }
    else {
        decodeBuffer(ret);
    }
}

int Input::getKey() {
    return key;
}

void Input::decodeBuffer(int size) {
    std::cout << "BUFFER: ";
    for (int i=0;i<size;i++) {
        std::cout << (int)buffer[i] << ';';
    }
    std::cout << CRLF;
    key = buffer[0];
}