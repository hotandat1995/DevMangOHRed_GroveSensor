#include <legato.h>

class testing{
public:
    testing();
    void print();
    ~testing();
};

testing::testing(){
    LE_INFO("Contructor");
}

testing::~testing(){
    LE_INFO("Destructor");
}

void testing::print(){
    LE_INFO("Print some thing!!!");
}

COMPONENT_INIT{
    testing muahaha;
    muahaha.print();
}