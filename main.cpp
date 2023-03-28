#include <stdlib.h>
#include <stdio.h>
#include "vm.hpp"

int main(int argc, char** argv)
{
    lc3vm::VM vm;
    if(argc != 2)
    {
        std::cout << "Wrong number of arguments" << std::endl;
        exit(1);
    }
    vm.loadProgram(argv[1]);
    vm.run();
    return EXIT_SUCCESS;
}