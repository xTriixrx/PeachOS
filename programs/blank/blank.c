#include "stdlib.h"

int main(int argc, char** argv)
{
    print("Hello World!\n");

    while(1) {
        if (getkey() != 0)
        {
            print("Key was pressed!\n");
        }
    }

    return 0;
}