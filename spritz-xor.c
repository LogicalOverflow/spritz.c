/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org>
 */
#include "spritz-xor.h"

#define SWAP(i, j) (i ^= j); (j ^= i); (i ^= j)
#define LOBYTE(b) ((uint8_t)(b & 0x0F))
#define HIBYTE(b) ((uint8_t)(b >> 4))
#define N 256

uint8_t i, j, k, w, z, a;
uint8_t s[N];

static void update() {
    i += w;
    j = k + s[j + s[i]];
    k = i + k + s[j];
    SWAP(s[i], s[j]);
}

static void whip() {
    for (uint32_t v = 0; v < (N * 2); v++) {
        update();
    }
    w += 2;
}

static void crush() {
    for (uint32_t v = 0; v < (N / 2); v++) {
        if (s[v] > s[N - 1 - v]) {
            SWAP(s[v], s[N - 1 - v]);
        }
    }
}

static void shuffle() {
    whip();
    crush();
    whip();
    crush();
    whip();
    a = 0;
}

static inline uint8_t output() {
    z = s[j + s[i + s[z + k]]];  
    return z;
}

static inline uint8_t drip() {
    if (a > 0) {
        shuffle();
    }
    update();
    return output();
}

static void absorb_nibble(uint8_t x) {
    if (a < (N / 2)) {
        shuffle();
    }
    SWAP(s[a], s[(N / 2) + x]);
    a++;
}

static inline void absorb_byte(uint8_t b) {
    absorb_nibble(LOBYTE(b));
    absorb_nibble(HIBYTE(b));    
}

static inline void absorb(const uint8_t *k, size_t ks) {
    for (size_t v = 0; v < ks; v++) {
        absorb_byte(k[v]);
    }    
}

static inline void initialize_state() {
    i = j = k = z = a = 0; w = 1;
    for (uint32_t v = 0; v < N; v++) {
        s[v] = (uint8_t)v;
    }    
}

static inline void key_setup(const uint8_t *k, size_t ks) {
    initialize_state();
    absorb(k, ks);
}

extern void spritz_crypt(uint8_t *m, size_t ms, const uint8_t *k, size_t ks) {
    key_setup(k, ks);

    for (size_t v = 0; v < ms; v++) {
        m[v] ^= drip();
    }
}
