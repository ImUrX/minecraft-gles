#include "srand.c"
#include <stdio.h>

#define SIZE 0x10000000

int main(int argc, char *argv[]) {
    SimpleRandom r;
    unsigned int slot1 = 5, slot2 = 20, slot3 = 30;
    for(unsigned int shelves = 0; shelves < 16; shelves++) {
        set_seed(&r, new_ulong(-2147482862, 0xffffffff));
        unsigned int slot1gen = generic_enchantability(&r, shelves) / 3;
        if(slot1gen < 1) slot1gen =  1;
        printf("%d ", slot1gen);
        //if(slot1 != slot1gen) return 1;
        unsigned int slot2gen = (generic_enchantability(&r, shelves) * 2 / 3) + 1;
        printf("%d ", slot2gen);
        //if(slot2 != slot2gen) return 1;
        unsigned int slot3gen;
        {
            unsigned int gen = (generic_enchantability(&r, shelves));
            unsigned int double_shelves = shelves * 2;
            slot3gen = gen > double_shelves ? gen : double_shelves;
        }
        printf("%d\n", slot3gen);
        if(slot3 == slot3gen) {
            return 0;
        }
    }
}