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
	uint32_t a,b,c,d,min,max;
    a = x.low >> 16;
    b = x.low & MSK16;
    c = y.low >> 16;
    d = y.low & MSK16;
    z.low = x.low * y.low;
    if(x.low > y.low) {
        min = y.low;
        max = x.low;
    } else {
        max = y.low;
        min = x.low;
    }
    z.high = (x.high * y.low) + (x.low * y.high) + a*c + ((a*d + b*c) >> 16) + (z.low < max && min >= 0xffffUL);
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

uint32_t next_int(SimpleRandom *self) {
    self->seed = andu64(addu64(multu64(self->seed, MULT), get_ulong(0xBU)), MASK);
    return rshiftu64(self->seed, 17, 131072, 32768).low;
}

uint32_t next_int_bound(SimpleRandom *self, uint32_t bound) {
    uint32_t r = next_int(self);
    uint32_t m = bound - 1;
    if((bound & m) == 0) {
        r = rshiftu64(multu64(get_ulong(bound), get_ulong(r)), 31, 2147483648, 2).low;
    } else {
        uint32_t u = r;
        while(u - (r = u % bound) + m < 0) u = next_int(self);
    }
    return r;
}

uint32_t generic_enchantability(SimpleRandom *self, uint32_t shelves) {
    uint32_t first = next_int_bound(self, 8);
    uint32_t second = next_int_bound(self, shelves + 1);
    return first + 1 + (shelves >> 1) + second;
}