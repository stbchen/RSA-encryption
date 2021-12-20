#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

// help()
// help() prints out the program usage and help.
void help(void) {
    fprintf(stdout, "SYNOPSIS\n\
   Encrypts data using RSA encryption.\n\
   Encrypted data is decrypted by the decrypt program.\n\
\n\
USAGE\n\
   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\
\n\
OPTIONS\n\
   -h              Display program help and usage.\n\
   -v              Display verbose program output.\n\
   -i infile       Input file of data to encrypt (default: stdin).\n\
   -o outfile      Output file for encrypted data (default: stdout).\n\
   -n pbfile       Public key file (default: rsa.pub).\n");
    return;
}

// main()
// main() opens the input file and the public key, and encrypts the input file and saves to the output file.
int main(int argc, char **argv) {

    // opt for getopt
    int opt = 0;

    // Booleans for command line options
    bool stats = false;
    // Files to use
    FILE *infile = stdin;
    FILE *outfile = stdout;

    // Path of public key
    char *keypath = "rsa.pub";
    // gets all command line options
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help(); return 0;
        case 'v': stats = true; break;
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error: failed to open infile.\n");
                return 1;
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error: failed to open outfile.\n");
                return 1;
            }
            break;
        case 'n': keypath = optarg; break;
        default: help(); return 1;
        }
    }

    // Read the public key
    FILE *pubkey = fopen(keypath, "r");

    // Check if public key is valid
    if (pubkey == NULL) {
        fprintf(stderr, "Error: failed to open public key.\n");
        return 1;
    }
    // Initalize variables
    mpz_t n, e, s, user;
    mpz_inits(n, e, s, user, NULL);
    char username[1024];
    size_t prbits;

    // Read public key
    rsa_read_pub(n, e, s, username, pubkey);
    // Verbose printing
    if (stats) {
        gmp_fprintf(stdout, "user = %s\n", username);
        prbits = mpz_sizeinbase(s, 2);
        gmp_fprintf(stdout, "s (%zu bits) %Zd\n", prbits, s);
        prbits = mpz_sizeinbase(n, 2);
        gmp_fprintf(stdout, "n (%zu bits) %Zd\n", prbits, n);
        prbits = mpz_sizeinbase(e, 2);
        gmp_fprintf(stdout, "e (%zu bits) %Zd\n", prbits, e);
    }

    // Convert username to an mpz_t
    mpz_set_str(user, username, 62);

    // Verify if signature is valid
    if (!rsa_verify(user, s, e, n)) {
        fprintf(stderr, "Error: Couldn't verify signature.\n");
        mpz_clears(n, e, s, user, NULL);
        fclose(infile);
        fclose(outfile);
        fclose(pubkey);
        return 1;
    }

    // Encrypt the file
    rsa_encrypt_file(infile, outfile, n, e);

    // Clear mpz_t and close files, exit program
    mpz_clears(n, e, s, user, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pubkey);
    return 0;
}
