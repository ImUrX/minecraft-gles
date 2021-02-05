#include <stdio.h>
#include <stdint.h>

#define MSK32 0xffffffffU
#define MSK16 0xffffU


typedef struct _ulong {
	uint32_t low;
	uint32_t high;
} ulong;

ulong new_ulong(uint32_t low, uint32_t high) {
    ulong x;
    x.low = low;
    x.high = high;
    return x;
}

static const ulong MULT = {3740067437, 5};
static const ulong MASK = {0xffffffff, 0xffff};

ulong get_ulong(uint32_t a) {
    return new_ulong(a, 0);
}

ulong addu64(ulong a, ulong b) {
	ulong c;
	c.low = (a.low + b.low) & MSK32;
	c.high = (a.high + b.high + (c.low < a.low)) & MSK32;
	return c;
}

//Based on https://stackoverflow.com/questions/1541426/computing-high-64-bits-of-a-64x64-int-product-in-c/1546152#1546152
void mixed(ulong *x, uint32_t inner_term) {
    x->high += inner_term >> 16;
    uint32_t was = x->low;
    x->low += (inner_term & MSK16) << 16;

    if(x->low < was) ++x->high;
}

ulong multu64(ulong x, ulong y) {
	ulong z;
	uint32_t a,b,c,d;
    a = x.low >> 16;
    b = x.low & MSK16;
    c = y.low >> 16;
    d = y.low & MSK16;
    z.high = a * c;
    z.low = b * d;
    mixed(&z, a * d);
    mixed(&z, b * c);
	return z;
}

ulong xoru64(ulong a, ulong b) {
    ulong c;
    c.low = a.low ^ b.low;
    c.high = a.high ^ b.high;
	return c;
}

ulong andu64(ulong a, ulong b) {
	ulong c;
    c.low = a.low & b.low;
    c.high = a.high & b.high;
	return c;
}

ulong rshiftu64(ulong a, uint32_t b, uint32_t twopowb, uint32_t bpowinv) { //max amount of shifting is 31 i think xd
	ulong c = a;
	c.low = (a.low >> b) + ((a.high % twopowb) * bpowinv);
	c.high = a.high >> b;
	return c;
}

void print_ulong(ulong x) {
    printf("%u, %u\n", x.low, x.high);
}

typedef struct _Random {
    ulong seed;
} SimpleRandom;

void set_seed(SimpleRandom *self, ulong seed) {
    self->seed = andu64(xoru64(seed, MULT), MASK);
}

unsigned int next_int(SimpleRandom *self) {
    print_ulong(multu64(self->seed, MULT));
    self->seed = andu64(addu64(multu64(self->seed, MULT), get_ulong(0xBU)), MASK);
    return rshiftu64(self->seed, 17, 131072, 32768).low;
}

unsigned int next_int_bound(SimpleRandom *self, unsigned int bound) {
    unsigned int r = next_int(self);
    unsigned int m = bound - 1;
    if((bound & m) == 0) {
        r = (((unsigned long) bound) * ((unsigned long) r)) >> 31;
    } else {
        unsigned int u = r;
        while(u - (r = u % bound) + m < 0) u = next_int(self);
    }
    return r;
}

unsigned int generic_enchantability(SimpleRandom *self, unsigned int shelves) {
    unsigned int first = next_int_bound(self, 8);
    unsigned int second = next_int_bound(self, shelves + 1);
    return first + 1 + (shelves >> 1) + second;
}