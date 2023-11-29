#include <queue>

typedef unsigned char AChar;

enum KeyType {
    ERROR,
    STANDARD
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