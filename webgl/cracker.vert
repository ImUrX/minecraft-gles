#version 300 es

#define shelves 15
#define slot1 5
#define slot2 20
#define slot3 30

#define MSK32 0xffffffff

struct ulong {
	uint low;
	uint high;
};

ulong addu64(ulong a, ulong b) {
	ulong c = ulong(0, 0);
	c.low = (a.low + b.low) & MSK32;
	c.high = (a.high + b.high + (c.low < a.low)) & MSK32;
	return c;
}

ulong multu64(ulong a, ulong b) {
	ulong c = a;
	for(ulong i = ulong(0, 0); i.low < b.low || i.high < b.high; i = addu64(i, ulong(1, 0))) {
		c = addu64(c, b);
	}
	return c;
}

ulong xoru64(ulong a, ulong b) {
	return ulong(a.low ^ b.low, a.high ^ b.high);
}

ulong andu64(ulong a, ulong b) {
	return ulong(a.low & b.low, a.high & b.high);
}

ulong rshiftu64(ulong a, uint b, uint bpow2, uint bpowinv) { //max amount of shifting is 31 i think xd
	ulong c = a;
	c.low = (a.low >> b) + ((a.high % bpow2) * bpowinv);
	c.high = a.high >> b;
	return c;
}

struct Random {
	int high;
	int mid;
	int low;
};

void set_internal_seed(inout Random random, int state) {
	random.high = (state >> 32);
	random.mid = (state >> 16);
	random.low = state;
}

in uint seed;

out bool exists;

void main() {
    
}