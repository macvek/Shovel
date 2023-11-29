#include <queue>

struct Key {
    unsigned short value;
};

class InputDecoder {
    std::queue<Key> queue;

    public:
    void feed(char* buffer, int size);
    bool canLoad();
    Key load();
};