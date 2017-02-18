spritz.c
========
An `Spritz` cipher implementation in ANSI C.

Spritz is a RC4 redesign by Ron Rivest and Jacob Schuldt
[(PDF)](https://people.csail.mit.edu/rivest/pubs/RS14.pdf).

Exports
-------
* `void spritz_encrypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms)`
* `void spritz_decrypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms)`
* `void spritz_crypt(const uint8_t *k, size_t ks, uint8_t *m, size_t ms)`
* `void spritz_hash(const uint8_t *m, size_t ms, uint8_t *r, size_t rs)`

License
-------
Release into the [Public Domain](LICENSE).
