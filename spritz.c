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
#include "spritz.h"

#define SWAP(i, j) (t) = (i); (i) = (j); (j) = (t);
#define LOBYTE(b) ((uint8_t)((b) & 0x0F))
#define HIBYTE(b) ((uint8_t)((b) >> 4))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define S(a) s[(uint8_t)(a)]
#define N 256

uint8_t a, i, j, k, t, w, z, s[N];

static void update() {
    i += w;
    j = k + S(j + S(i));
    k = i + k + S(j);
    SWAP(S(i), S(j));
}

static void crush() {
    uint8_t v;
    for (v = 0; v < (N / 2); v++) {
        if (S(v) > S(N - 1 - v)) {
            SWAP(S(v), S(N - 1 - v));
        }
    }
}

static void whip() {
    uint32_t v;
    for (v = 0; v < (N * 2); v++) {
        update();
    }
    w += 2;
}

static uint8_t output() {
    z = S(j + S(i + S((z + k))));
    return z;
}

static void shuffle() {
    whip();
    crush();
    whip();
    crush();
    whip();
    a = 0;
}

static uint8_t drip() {
    if (a > 0) {
        shuffle();
    }
    update();
    return output();
}

static void squeeze(uint8_t *r, size_t rs) {
    size_t v;
    if (a > 0) {
        shuffle();
    }
    for (v = 0; v < MIN(rs, N); v++) {
        r[v] = drip();
    }
}

static void absorb_nibble(uint8_t x) {
    if (a == (N / 2)) {
        shuffle();
    }
    SWAP(S(a), S((N / 2) + x));
    a++;
}

static void absorb_stop() {
    if (a == (N / 2)) {
        shuffle();
    }
    a++;    
}

static void absorb_byte(uint8_t b) {
    absorb_nibble(LOBYTE(b));
    absorb_nibble(HIBYTE(b));    
}

static void absorb(const uint8_t *b, size_t bs) {
    size_t v;
    for (v = 0; v < bs; v++) {
        absorb_byte(b[v]);
    }    
}

static void initialize_state() {
    uint32_t v;
    a = i = j = k = z = 0; w = 1;
    for (v = 0; v < N; v++) {
        S(v) = (uint8_t)v;
    }    
}

static void key_setup(const uint8_t *k, size_t ks) {
    initialize_state();
    absorb(k, ks);
}

extern void spritz_encrypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms) {
    size_t v;
    key_setup(k, ks);
    for (v = 0; v < ms; v++) {
        m[v] += drip();
    }
}

extern void spritz_decrypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms) {
    size_t v;
    key_setup(k, ks);
    for (v = 0; v < ms; v++) {
        m[v] -= drip();
    }
}

extern void spritz_crypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms) {
    size_t v;
    key_setup(k, ks);
    for (v = 0; v < ms; v++) {
        m[v] ^= drip();
    }
}

extern void spritz_hash(const uint8_t *m, size_t ms, uint8_t *r, size_t rs) {
    initialize_state();
    absorb(m, ms);
    absorb_stop();
    absorb((uint8_t *)&rs, 1);
    squeeze(r, rs);
}
