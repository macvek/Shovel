#include <vector>

struct Key {

};

class InputDecoder {
    std::vector<Key> queue;

    public:
    void feed(char* buffer, int size);
    bool canLoad();
    Key load();
};