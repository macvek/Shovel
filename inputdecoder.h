#include <queue>

enum KeyType {
    STANDARD,
    ERROR
};

struct Key {
    unsigned short value;
    KeyType type;
    
};

class InputDecoder {
    std::queue<Key> queue;

    public:
    void feed(char* buffer, int size);
    bool canLoad();
    Key load();
};