#version 300 es

#define shelves 15
#define slot1 5
#define slot2 20
#define slot3 30

#define MSK32 0xffffffff
#define MSK16 0xffff

struct ulong {
	uint32_t low;
	uint32_t high;
};

const ulong MULT = {3740067437, 5};
const ulong MASK = {0xffffffffu, 0xffff};

ulong addu64(ulong a, ulong b) {
	return ulong(
		(a.low + b.low) & MSK32,
		(a.high + b.high + (c.low < a.low)) & MSK32
	);
}

ulong multu64(ulong x, ulong y) {
	ulong z = ulong(0, 0);
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
    z.high = (x.high * y.low) + (x.low * y.high) + a*c + ((a*d + b*c) >> 16) + (z.low < max && min >= MSK16);
	return z;
}

ulong xoru64(ulong a, ulong b) {
    return ulong(a.low ^ b.low, a.high ^ b.high);
}

ulong andu64(ulong a, ulong b) {
	return ulong(a.low & b.low, a.high & b.high);
}

ulong rshiftu64(ulong a, uint32_t b, uint32_t twopowb, uint32_t bpowinv) { //max amount of shifting is 31 i think xd
	ulong c = a;
	c.low = (a.low >> b) + ((a.high % twopowb) * bpowinv);
	c.high = a.high >> b;
	return c;
}

struct SimpleRandom {
    ulong seed;
};

void set_seed(inout SimpleRandom self, ulong seed) {
    self->seed = andu64(xoru64(seed, MULT), MASK);
}

uint32_t next_int(inout SimpleRandom self) {
    self->seed = andu64(addu64(multu64(self->seed, MULT), ulong(0xB, 0)), MASK);
    return rshiftu64(self->seed, 17, 131072, 32768).low;
}

uint32_t next_int_bound(inout SimpleRandom self, uint32_t bound) {
    uint32_t r = next_int(self);
    uint32_t m = bound - 1;
    if((bound & m) == 0) {
        r = rshiftu64(multu64(ulong(bound, 0), ulong(r, 0)), 31, 2147483648u, 2).low;
    } else {
        uint32_t u = r;
        while(u - (r = u % bound) + m < 0) u = next_int(self);
    }
    return r;
}

uint32_t generic_enchantability(inout SimpleRandom self, uint32_t shelves) {
    uint32_t first = next_int_bound(self, 8);
    uint32_t second = next_int_bound(self, shelves + 1);
    return first + 1 + (shelves >> 1) + second;
}

in uint seed;

out bool exists;

void main() {
    
}