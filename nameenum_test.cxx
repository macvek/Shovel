#include "nameenum.h"
#include <iostream>

enum MyEnum {
    AA,
    BB,
    CC
};

std::string nameIt(MyEnum myCustomValue) {
    NAMEENUM(myCustomValue, MyEnum, AA);

    return "Default";
}

int main() {
    MyEnum myCustomValue = AA;
    std::string named = nameIt(myCustomValue);
    if ("AA" != named) {
        std::cerr << "GOT " << named << " instead of AA" << std::endl;
        return 1;
    }


    return 0;
}