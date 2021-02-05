#ifndef __JRAND_CL
#define __JRAND_CL

// #define JRAND_DOUBLE
#define JRAND_SMOL

#define RANDOM_MULTIPLIER_LONG 0x5DEECE66DUL

#ifdef JRAND_DOUBLE
#define Random double
#define RANDOM_MULTIPLIER 0x5DEECE66Dp-48
#define RANDOM_ADDEND 0xBp-48

void set_internal_seed(Random *random, unsigned long state) {
	*random = (state * 0x1p-48);
}

unsigned int random_next (Random *random, int bits) {
	*random = trunc(fma(*random, RANDOM_MULTIPLIER, RANDOM_ADDEND) * 0x1p-48);
	return (unsigned int)((unsigned long)(*random / 0x1p-48) >> (48 - bits));
}

#else
#ifdef JRAND_SMOL

typedef struct _Random {
	unsigned short high;
	unsigned short mid;
	unsigned short low;
} Random;

#define RANDOM_MULTIPLIER_HIGH 0x0005U
#define RANDOM_MULTIPLIER_MID 0xdeecU
#define RANDOM_MULTIPLIER_LOW 0xe66dU
#define RANDOM_ADDEND 0xbU


void set_internal_seed(Random *random, unsigned long state) {
	random->high = (unsigned short) (state >> 32);
	random->mid = (unsigned short) (state >> 16);
	random->low = (unsigned short) state;
}

// Random::next(bits)
unsigned int random_next (Random *random, int bits) {
	unsigned short a32 = random->high;
	unsigned short a16 = random->mid;
	unsigned short a00 = random->low;
	
	unsigned int c32 = 0, c16 = 0, c00 = 0;
	c00 += a00 * RANDOM_MULTIPLIER_LOW;
	c16 += c00 >> 16;
	c00 &= 0xFFFF;
	c16 += a16 * RANDOM_MULTIPLIER_LOW;
	c32 += c16 >> 16;
	c16 &= 0xFFFF;
	c16 += a00 * RANDOM_MULTIPLIER_MID;
	c32 += c16 >> 16;
	c16 &= 0xFFFF;
	c32 += a32 * RANDOM_MULTIPLIER_LOW + a16 * RANDOM_MULTIPLIER_MID + a00 * RANDOM_MULTIPLIER_HIGH;
	c32 &= 0xFFFF;
		
	c00 += RANDOM_ADDEND;
	c16 += c00 >> 16;
	c00 &= 0xFFFF;
	c32 += c16 >> 16;
	
	c16 &= 0xffff;
	c32 &= 0xffff;
	
	random->high = (unsigned short) c32;
	random->mid = (unsigned short) c16;
	random->low = (unsigned short) c00;
	
	int shift = 48 - bits;
	if (shift >= 32) return c32 >> (shift - 32);
	if (shift >= 16) return (c32 << (32 - shift)) | (c16 >> (shift - 16));
	c00 &= 0xffff;
	c00 |= c16 << 16;
	return (c32 << (32 - shift)) | (c00 >> shift);
}

#else

#define Random unsigned long
#define RANDOM_MULTIPLIER RANDOM_MULTIPLIER_LONG
#define RANDOM_ADDEND 0xBUL
#define RANDOM_MASK (1UL << 48) - 1

void set_internal_seed(Random *random, unsigned long state) {
	*random = state;
}

// Random::next(bits)
unsigned int random_next (Random *random, int bits) {
	*random = (*random * RANDOM_MULTIPLIER + RANDOM_ADDEND) & RANDOM_MASK;
	return (unsigned int)(*random >> (48 - bits));
}
#endif // ~JRAND_SMOL
#endif // ~JRAND_DOUBLE

// new Random(seed)
#define set_seed(random, seed) set_internal_seed(random, (seed) ^ RANDOM_MULTIPLIER_LONG)

// Random::nextInt(bound)
unsigned int random_next_int (Random *random, unsigned int bound) {
	unsigned int r = random_next(random, 31);
	unsigned int m = bound - 1;
	if ((bound & m) == 0) {
			r = (unsigned int)((bound * (unsigned long)r) >> 31);
	} else {
			for (int u = r;
					 u - (r = u % bound) + m < 0;
					 u = random_next(random, 31));
	}
	return r;
}

long random_next_long (Random *random) {
	return (((long)random_next(random, 32)) << 32) + random_next(random, 32);
}

unsigned int random_next_int_no_pow2(Random *random, unsigned int bound) {
		int r = random_next(random, 31);
		int m = bound - 1;
		
		for (int u = r;
				u - (r = u % bound) + m < 0;
				u = random_next(random, 31));
			/*  
		unsigned int u = r;
		r = u % bound;
		while (u - r + m < 0) {
			u = random_next(random, 31);
			r = u % bound;
		}*/
		return r;
}

unsigned int random_next_int_24(Random *random) {
		return random_next_int(random, 24); /*
		unsigned int u = random_next(random, 31);
		unsigned int r = u % 24;
		while (u - r + 23 < 0) {
			u = random_next(random, 31);
			r = u % 24;
		}
		return r; */
}

unsigned int random_next_int_pow2(Random *random, unsigned int boundLog2) {
	return random_next(random, 31 - boundLog2);
}

#endif