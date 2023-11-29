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
    int k = buffer[0];
    
    std::cout << "EXTRA: " << size <<" => ";
    for (int i=0;i<size;i++) {
        int each = buffer[i];
        std::cout << each <<";";
    }
    std::cout << CRLF;
    
    
    key = k;
    
}
