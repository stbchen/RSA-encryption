# Assignment 6: "Public Key Cryptography"
This program will perform RSA public key cryptography. This program can generate public and private keys, encrypt and decrypt using the public and private keys. RSA encryption uses various math concepts like large primes numbers and modulus.

## Building

Build the program with:

To build encrypt:
```
$ make encrypt
```

To build decrypt:
```
$ make decrypt

```

To build keygen:
```
$ make keygen
```
or

to build all:
```
$ make all
```

To format:
```
$ make format
```

To clean files:
```
$ make clean
```

## Running

Run encrypt with (including command line options):
```
$ ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey
```
Command line options for encrypt:
   -h              Display program help and usage.
   -v              Display verbose program output.
   -i infile       Input file of data to encrypt (default: stdin).
   -o outfile      Output file for encrypted data (default: stdout).
   -n pbfile       Public key file (default: rsa.pub).

Run decrypt with (including command line options):
```
$ ./decrypt [-hv] [-i infile] [-o outfile] -n privkey
```

Command line options for decrypt:
   -h              Display program help and usage.
   -v              Display verbose program output.
   -i infile       Input file of data to decrypt (default: stdin).
   -o outfile      Output file for decrypted data (default: stdout).
   -n pvfile       Private key file (default: rsa.priv).

Run keygen with (including command line options):
```
$ ./keygen [-hv] [-b bits] [-i confidence] [-n pbfile] [-d pvfile] -s seed
```

Command line options for keygen:
   -h              Display program help and usage.
   -v              Display verbose program output.
   -b bits         Minimum bits needed for public key n.
   -i confidence   Miller-Rabin iterations for testing primes (default: 50).
   -n pbfile       Public key file (default: rsa.pub).
   -d pvfile       Private key file (default: rsa.priv).
   -s seed         Random seed for testing.

