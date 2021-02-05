#include "srand.c"
#include <stdio.h>

#define SIZE 0x10000000

int main(int argc, char *argv[]) {
    SimpleRandom r;
    unsigned int shelves = 15, slot1 = 5, slot2 = 20, slot3 = 30;
    set_seed(&r, new_ulong(-2147482862, 0xffffffff));
    unsigned int slot1gen = generic_enchantability(&r, shelves) / 3;
    if(slot1gen < 1) slot1gen =  1;
    printf("%d\n", slot1gen);
    if(slot1 != slot1gen) return 1;
    unsigned int slot2gen = (generic_enchantability(&r, shelves) * 2 / 3) + 1;
    if(slot2 != slot2gen) return 1;
    unsigned int slot3gen;
    {
        unsigned int gen = (generic_enchantability(&r, shelves));
        unsigned int double_shelves = shelves * 2;
        slot3gen = gen > double_shelves ? gen : double_shelves;
    }
    if(slot3 == slot3gen) {
        printf("%u\n", -2147482862);
    }
}