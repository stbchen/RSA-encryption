#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <limits.h>

#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

// Command line options
#define OPTIONS "b:i:n:d:s:vh"

// help()
// Parameters: None
// Returns: N/A
// Prints out the program help and info message.
void help(void) {
    fprintf(stdout, "SYNOPSIS\n\
   Generates an RSA public/private key pair.\n\
\n\
USAGE\n\
   ./keygen [-hv] [-b bits] [-i confidence] [-n pbfile] [-d pvfile] -s seed\n\
\n\
OPTIONS\n\
   -h              Display program help and usage.\n\
   -v              Display verbose program output.\n\
   -b bits         Minimum bits needed for public key n.\n\
   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n\
   -n pbfile       Public key file (default: rsa.pub).\n\
   -d pvfile       Private key file (default: rsa.priv).\n\
   -s seed         Random seed for testing.\n");
    return;
}

// file_perm()
// Parameters: editfile
// FILE *editfile: File to edit permissions too
// Returns: N/A
// file_perm() takes in a file and sets its permissions to 600
void file_perm(FILE *editfile) {

    // Get the file number
    int filenum = 0;
    filenum = fileno(editfile);

    // Change file permissions to 600
    fchmod(filenum, 0600);
    return;
}

// main()
// main() takes in command line options, opens/creates files for public and private keys for RSA encryption.
int main(int argc, char **argv) {

    // opt for getopt
    int opt = 0;

    // Path of file (From Eugene's 11/16 section)
    char *pbpath = "rsa.pub";
    char *pvpath = "rsa.priv";

    // The booleans for command line options
    bool stats = false;

    // uints for numtheory and rsa
    uint64_t bits = 256;
    uint64_t seed = time(NULL);
    uint64_t iters = 50; // Default is 50

    // gets all command line options
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help(); return 0;
        case 'v': stats = true; break;
        case 'b':
            bits = strtoul(optarg, NULL, 0);
            if (bits < 4) {
                fprintf(stderr, "Error: Number of bits is invalid.\n");
                return 1;
            }
            break;
        case 'i': iters = strtoul(optarg, NULL, 0); break;
        case 'n': pbpath = optarg; break;
        case 'd': pvpath = optarg; break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        default: help(); return 1;
        }
    }

    // The public/private files
    FILE *pbfile = fopen(pbpath, "w");
    FILE *pvfile = fopen(pvpath, "w");

    // Checking to see if files exist
    if (pbfile == NULL) {
        fprintf(stderr, "Error: failed to open pbfile.\n");
        fclose(pvfile);
        return 1;
    }

    if (pvfile == NULL) {
        fprintf(stderr, "Error: failed to open pvfile.\n");
        fclose(pbfile);
        return 1;
    }

    file_perm(pvfile);
    file_perm(pbfile);

    // Initialize randstate
    randstate_init(seed);

    // Initialize p and q (prime numbers), n (product of p * q),
    // e (public exponent), d (private key), user (the username),
    // and s (the signature), prbits (bits for printing)
    mpz_t p, q, n, e, d, user, s;
    size_t prbits = 0;
    mpz_inits(p, q, n, e, d, user, s, NULL);

    // Make public key
    rsa_make_pub(p, q, n, e, bits, iters);

    // Make private key
    rsa_make_priv(d, e, p, q);

    // Char array for username
    char *username[sizeof(getenv("USER"))];
    // Get the user's name and set to string
    *username = getenv("USER");
    mpz_set_str(user, *username, 62);

    // Use username to sign
    rsa_sign(s, user, d, n);

    // Write public key to file
    rsa_write_pub(n, e, s, *username, pbfile);

    // Write private key to file
    rsa_write_priv(n, d, pvfile);

    // Verbose printing
    if (stats) {
        gmp_fprintf(stdout, "user = %s\n", *username);
        prbits = mpz_sizeinbase(s, 2);
        gmp_fprintf(stdout, "s (%zu bits) %Zd\n", prbits, s);
        prbits = mpz_sizeinbase(p, 2);
        gmp_fprintf(stdout, "p (%zu bits) %Zd\n", prbits, p);
        prbits = mpz_sizeinbase(q, 2);
        gmp_fprintf(stdout, "q (%zu bits) %Zd\n", prbits, q);
        prbits = mpz_sizeinbase(n, 2);
        gmp_fprintf(stdout, "n (%zu bits) %Zd\n", prbits, n);
        prbits = mpz_sizeinbase(e, 2);
        gmp_fprintf(stdout, "e (%zu bits) %Zd\n", prbits, e);
        prbits = mpz_sizeinbase(d, 2);
        gmp_fprintf(stdout, "d (%zu bits) %Zd\n", prbits, d);
    }

    // Clear used mpz_t's, closes files,  and exits program
    mpz_clears(p, q, n, e, d, user, s, NULL);
    randstate_clear();
    fclose(pbfile);
    fclose(pvfile);
    return 0;
}
