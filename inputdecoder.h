#include <queue>

typedef unsigned char AChar;

enum KeyType {
    ERROR,
    STANDARD,
    SPECIALS, // MARKER for checking for special CHARACTERS by k.type > SPECIALS
    
    ENTER,
    TAB,
    ESCAPE,

    ARROW_UP,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT,
    
    END,
    HOME,

    F1,
    F2,
    F3,
    F4,
    
};

struct Key {
    AChar value;
    KeyType type;
    
};

class InputDecoder {
    std::queue<Key> queue;

    public:
    void feed(AChar* buffer, int size);
    bool canLoad();
    Key load();
};