#include <stdio.h>
#include <string.h>
#include <assert.h>

#define N_ITEMS 1000000
#define N_SEEKS 10000
#define N_LOOPS 20

static int strstrcmp(const void *a, const void *b) {
	const char * const *x = a;
	const char * const *y = b;
	return strcmp(*x, *y);
}


#ifdef USE_KHASH

#include "../../../../klib/khash.h"

KHASH_MAP_INIT_STR(km, int)

static int* get_kh_value(khash_t(km) *b, const char *name) {
        khint_t k = kh_get(km, b, name);
        if(k == kh_end(b)) return 0;
        return &kh_value(b, k);
}

static int insert_kh(khash_t(km) *b, const char *name, int* m) {
        int absent;
        khint_t k = kh_put(km, b, name, &absent);
        if (!absent) {
        }
        kh_value(b, k) = *m;
        return !absent;
}


int test_khash() {
	khash_t(km) *b;
	b = kh_init(km);

	char buf[64];
	int i;
	for(i=0; i<N_ITEMS; i++) {
		sprintf(buf, "%d", i);
		const char *key = strdup(buf);
		insert_kh(b, key, &i);
	}
	for(i=0; i<N_ITEMS; i++) {
		sprintf(buf, "%d", i);
		const char *key = buf;
		assert(get_kh_value(b, key));
	}

	srand(0);
	int res = 0;
	for(i=0; i<N_SEEKS; i++) {
		int val = rand()%N_ITEMS;
		sprintf(buf, "%d", val);
		const char *key = buf;
		res += *(int*)get_kh_value(b, key);
	}
	return res;
}
#else

#include "../include/hbmap.h"

static inline unsigned __ac_X31_hash_string(const char *s)
{
	unsigned h = (unsigned)*s;
	if (h) for (++s ; *s; ++s) h = (h << 5) - h + (unsigned)*s;
	return h;
}


int test_bmap() {
	hbmap(char*, int, 256) b_b, *b = &b_b;
	hbmap_init(b, strstrcmp, __ac_X31_hash_string);

	char buf[64];
	int i;
	for(i=0; i<N_ITEMS; i++) {
		sprintf(buf, "%d", i);
		const char *key = strdup(buf);
		hbmap_insert_nocheck(b, key, i);
	}

	// check that hbmap_insert works, too */
	i = 0; sprintf(buf, "0");
	const char *key = strdup(buf);
	//assert(hbmap_insert(b,  key, i) == 0);

	for(i=0; i<N_ITEMS; i++) {
		sprintf(buf, "%d", i);
		const char *key = buf;
		assert(hbmap_contains(b, key));
	}

	srand(0);
	int res = 0;
	for(i=0; i<N_SEEKS; i++) {
		int val = rand()%N_ITEMS;
		sprintf(buf, "%d", val);
		const char *key = buf;
		res += *hbmap_get(b, key);
	}

	hbmap_fini(b, 1);
	return res;
}


static int intcmp(const void *a, const void *b) {
	int const *x = a;
	int const *y = b;
	return *x - *y;
}

static inline unsigned int_hash(int x) { return x; }

int test_bmap_int() {
	hbmap(int, int, 256) b_b, *b = &b_b;
	hbmap_init(b, intcmp, int_hash);

	int i;
	for(i=0; i<N_ITEMS; i++) {
		hbmap_insert(b, i, i);
	}
	for(i=0; i<N_ITEMS; i++) {
		assert(hbmap_contains(b, i));
	}

	srand(0);
	int res = 0;
	for(i=0; i<N_SEEKS; i++) {
		int val = rand()%N_ITEMS;
		res += *hbmap_get(b, val);
	}
	return res;
}

#endif

int main(int argc, char**argv) {
	int i;
#ifdef USE_KHASH

	if(argc > 1 && !strcmp(argv[1], "khash"))
		for(i=0; i< N_LOOPS; i++)
			printf("%d\n", test_khash());
#else
//	else
		for(i=0; i< N_LOOPS; i++)
			printf("%d\n", test_bmap());
#endif

}
