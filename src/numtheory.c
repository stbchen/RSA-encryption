#include <stdio.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"

#include <stdbool.h>
#include <stdint.h>

// gcd()
// gcd() calculates the greatest common divisor
void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t t, na, nb;
    mpz_init(t);
    mpz_init_set(na, a);
    mpz_init_set(nb, b);
    while (mpz_cmp_ui(nb, 0) != 0) {
        mpz_set(t, nb);
        mpz_mod(nb, na, nb);
        mpz_set(na, t);
    }
    mpz_set(g, na);
    mpz_clears(t, na, nb, NULL);
    return;
}

// mod_inverse()
// mod_inverse() calculates the mod inverse
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    mpz_t r, rr, t, tt, q, temp, temp2;
    mpz_init_set(r, n);
    mpz_init_set(rr, a);
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(tt, 1);
    mpz_inits(q, temp, temp2, NULL);
    while (mpz_cmp_ui(rr, 0) != 0) {
        mpz_fdiv_q(q, r, rr);
        mpz_set(temp, r);
        mpz_set(r, rr);
        mpz_mul(temp2, q, rr);
        mpz_sub(rr, temp, temp2);
        mpz_set(temp, t);
        mpz_set(t, tt);
        mpz_mul(temp2, q, tt);
        mpz_sub(tt, temp, temp2);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        mpz_clears(r, rr, t, tt, q, temp, temp2, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(r, rr, t, tt, q, temp, temp2, NULL);
    return;
}

// pow_mod()
// pow_mod() calculates power mod
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_t v, nd, p;
    mpz_init_set(nd, d);
    mpz_init_set_ui(v, 1);
    mpz_init_set(p, a);
    while (mpz_cmp_ui(nd, 0) > 0) {
        if (mpz_odd_p(nd)) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(nd, nd, 2);
    }
    mpz_set(o, v);
    mpz_clears(v, p, nd, NULL);
}

// is_prime()
// is_prime() uses Miller Rabin primality test to check if a number is prime or not
bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t r, nminusone, a, y, j, two, bounds;
    mp_bitcnt_t s = 0;
    mpz_init_set_ui(r, 0);
    mpz_init_set_ui(two, 2);
    mpz_inits(nminusone, a, y, j, bounds, NULL);
    mpz_set_ui(a, 0);
    // Get n - 1
    mpz_sub_ui(nminusone, n, 1);
    // Checks base cases 0, 1, 3 and they are composite
    if (mpz_cmp_ui(n, 0) == 0 || mpz_cmp_ui(n, 1) == 0 || mpz_cmp_ui(n, 4) == 0) {
        mpz_clears(r, nminusone, a, y, j, two, bounds, NULL);
        return false;
    }

    // Checks base cases 2, 3 and they are prime
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        mpz_clears(r, nminusone, a, y, j, two, bounds, NULL);
        return true;
    }
    // Gets an r value that is odd
    while (mpz_even_p(r)) {
        mpz_tdiv_q_2exp(r, nminusone, s);
        mpz_fdiv_q_ui(r, r, 2);
        s++;
    }

    // Set s - 1 and the random end bounds (n-3);
    mp_bitcnt_t sminusone = s - 1;
    mpz_sub_ui(bounds, n, 3);

    // Loop until iterations is met
    for (uint64_t i = 1; i < iters; i++) {
        // Get a random number a that is between (2 to n - 3);
        mpz_urandomm(a, state, bounds);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, nminusone) != 0)) {
            mpz_set_ui(j, 1); // Set j to 1
            // Loop until j <= n - 1 and y != n - 1
            while (mpz_cmp_ui(j, sminusone) <= 0 && mpz_cmp(y, nminusone) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) { // If y = 1
                    mpz_clears(r, nminusone, a, y, j, two, bounds, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1); // Add 1 to j
            }
            if (mpz_cmp(y, nminusone) != 0) { // If y != n - 1
                mpz_clears(r, nminusone, a, y, j, two, bounds, NULL);
                return false;
            }
        }
    }
    mpz_clears(r, nminusone, a, y, j, two, bounds, NULL);
    return true;
}

// make_prime()
// make_prime() makes a prime number
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mp_bitcnt_t b = bits;
    while (true) {
        mpz_urandomb(p, state, b);
        if (bits == 1) {
            mpz_add_ui(p, p, 1);
        }
        if (is_prime(p, iters)) {
            return;
        }
    }
}
