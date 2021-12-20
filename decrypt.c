#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

/*
int main(void) {
    return 0;
}
*/

void help(void) {
    fprintf(stdout, "SYNOPSIS\n\
   Decrypts data using RSA decryption.\n\
   Encrypted data is encrypted by the encrypt program.\n\
\n\
USAGE\n\
   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n\
\n\
OPTIONS\n\
   -h              Display program help and usage.\n\
   -v              Display verbose program output.\n\
   -i infile       Input file of data to decrypt (default: stdin).\n\
   -o outfile      Output file for decrypted data (default: stdout).\n\
   -n pvfile       Private key file (default: rsa.priv).\n");
    return;
}

int main(int argc, char **argv) {

    // opt for getopt
    int opt = 0;

    // The booleans for command line options
    bool stats = false;
    char *pvpath = "rsa.priv";

    // The public/private files
    FILE *infile = stdin;
    FILE *outfile = stdout;

    // gets all command line options
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
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
        case 'n': pvpath = optarg; break;
        case 'v': stats = true; break;
        case 'h': help(); return 0;
        default: help(); return 1;
        }
    }

    FILE *pvfile = fopen(pvpath, "r");
    // Check if pvfile exists
    if (pvfile == NULL) {
        fprintf(stderr, "Error: failed to open private key.\n");
        fclose(infile);
        fclose(outfile);
        fclose(pvfile);
        return 1;
    }
    mpz_t n, e;
    mpz_inits(n, e, NULL);

    // Read the private key
    rsa_read_priv(n, e, pvfile);

    size_t prbits;
    if (stats) {
        prbits = mpz_sizeinbase(n, 2);
        gmp_fprintf(stdout, "n (%zu bits) %Zd\n", prbits, n);
        prbits = mpz_sizeinbase(e, 2);
        gmp_fprintf(stdout, "e (%zu bits) %Zd\n", prbits, e);
    }

    // Decrypt the file
    rsa_decrypt_file(infile, outfile, n, e);

    // Clear variables and close files
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);
    mpz_clears(n, e, NULL);

    // Exits the program
    return 0;
}
