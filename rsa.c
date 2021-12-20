#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define BLOCK 1024

// rsa_make_pub()
// rsa_make_pub() makes the public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t nq, np, totient, randexp, res;
    mpz_inits(np, nq, totient, randexp, res, NULL);

    // Loop until log2(n) = nbits
    // Create random # of bits for p and q, then make prime and n (p*q)
    do {
        uint64_t pbits = (random() % (nbits / 2)) + (nbits / 4);
        uint64_t qbits = nbits - pbits;
        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);
        mpz_mul(n, p, q);
    } while (mpz_sizeinbase(n, 2) != nbits);

    // Get p-1 and q-1 for totient calculation
    mpz_sub_ui(np, p, 1);
    mpz_sub_ui(nq, q, 1);
    mpz_mul(totient, np, nq);

    // Get an e where it is coprime to totient
    while (mpz_cmp_ui(res, 1) != 0) {
        mpz_urandomb(randexp, state, nbits);
        gcd(res, totient, randexp);
    }
    mpz_set(e, randexp);
    mpz_clears(np, nq, totient, randexp, res, NULL);
    return;
}

// rsa_write_pub()
// rsa_write_pub() writes the public key to a file
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
}

// rsa_read_pub()
// rsa_read_pub() reads the public key from a file
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
}

// rsa_make_priv()
// rsa_make_priv() makes the private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t n, np, nq;
    mpz_inits(n, np, nq, NULL);

    // Get q-1 and p-1
    mpz_sub_ui(np, p, 1);
    mpz_sub_ui(nq, q, 1);

    // Multiply (p-1)*(q-1) to get n
    mpz_mul(n, np, nq);

    // Get mod inverse
    mod_inverse(d, e, n);
    mpz_clears(n, nq, np, NULL);
    return;
}

// rsa_write_priv()
// rsa_write_priv() writes the private key to a file
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
}

// rsa_read_priv()
// rsa_read_priv() reads in a private key from a file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
}

// rsa_encrypt()
// rsa_encrypt() encrypts a message using the encryption formula
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

// rsa_encrypt_file()
// rsa_encrypt_file() encrypts a file
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);

    // Set j to 1 as the first bit is 0xFF
    uint64_t j = 1;
    int k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t fbit = 0xFF;

    // Dynamically allocate an array of uint8_t *
    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));

    // Set first value to 0xFF
    block[0] = fbit;

    // Loop until j is less than or equal to 0
    // read in the infile to get ciphertext
    // import ciphertext to mpz
    // encrypt the ciphertext
    // print the encrypted message to outfile
    while (j > 0) {
        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    // Clear mpz_t variables, free array and exit function
    mpz_clears(m, c, NULL);
    free(block);
    return;
}

// rsa_decrypt()
// rsa_decrypt() decrypts a message using the decryption formula
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

// rsa_decrypt_file()
// rsa_decrypt_file() decrypts an encrypted message
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {

    // Declare and initialize variables
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    uint64_t k = 0;
    uint64_t bytes = 1;

    // Calculate k
    k = (mpz_sizeinbase(n, 2) - 1) / 8;

    // Dynamically allocate an array of uint8_t *
    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));

    // Loop until entire file is scanned
    // Scan in encrypted message from infile
    // Decrypt the encoded message
    // Export to mpz
    // Write the decoded message to outfile
    while (!feof(infile)) {
        bytes = gmp_fscanf(infile, "%Zx\n", c);
        rsa_decrypt(m, c, d, n); // Decrypt ciphertext
        // Export block to message
        mpz_export(block, &bytes, 1, sizeof(uint8_t), 1, 0, m);
        // Write decrypted message to outfile
        fwrite((block + 1), sizeof(uint8_t), bytes - 1, outfile);
    }

    // Clear mpz_t variables, free array and exit function
    mpz_clears(c, m, NULL);
    free(block);
    return;
}

// rsa_sign()
// rsa_sign() creates the signature from a username for the public key
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

// rsa_verify()
// rsa_verify() verifies the signature in public key
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
