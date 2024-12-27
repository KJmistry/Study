#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <stdio.h>

#define KEY_SIZE             2048
#define EXPONENT             65537
#define RSA_KEY_SIZE         2048
#define SSL_PRIVATE_KEY_PATH "private_key.pem"

void EPRINT(const char *log_type, const char *message)
{
    fprintf(stderr, "%s: %s\n", log_type, message);
}

int main()
{
    EVP_PKEY     *pkey = NULL;
    EVP_PKEY_CTX *pKeyCtx = NULL;
    FILE         *pKeyFile = NULL;

    // 1. Generate RSA Key Pair using the provided method
    pkey = EVP_PKEY_new();
    if (NULL == pkey)
    {
        EPRINT("SYS_LOG", "error creating EVP_PKEY object");
        return 1;
    }

    pKeyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (NULL == pKeyCtx)
    {
        EPRINT("SYS_LOG", "error creating EVP_PKEY_CTX object");
        EVP_PKEY_free(pkey);
        return 1;
    }

    if (EVP_PKEY_keygen_init(pKeyCtx) <= 0)
    {
        EPRINT("SYS_LOG", "error initializing key generation");
        EVP_PKEY_CTX_free(pKeyCtx);
        EVP_PKEY_free(pkey);
        return 1;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pKeyCtx, RSA_KEY_SIZE) <= 0)
    {
        EPRINT("SYS_LOG", "error setting RSA key size");
        EVP_PKEY_CTX_free(pKeyCtx);
        EVP_PKEY_free(pkey);
        return 1;
    }

    if (EVP_PKEY_keygen(pKeyCtx, &pkey) <= 0)
    {
        EPRINT("SYS_LOG", "error generating key pair");
        EVP_PKEY_CTX_free(pKeyCtx);
        EVP_PKEY_free(pkey);
        return 1;
    }

    // Write the private key to a PEM file
    pKeyFile = fopen(SSL_PRIVATE_KEY_PATH, "wb");
    if (NULL == pKeyFile)
    {
        EPRINT("SYS_LOG", "unable to open private key file for writing");
        EVP_PKEY_CTX_free(pKeyCtx);
        EVP_PKEY_free(pkey);
        return 1;
    }

    if (PEM_write_PrivateKey(pKeyFile, pkey, NULL, NULL, 0, NULL, NULL) != 1)
    {
        EPRINT("SYS_LOG", "error writing private key to file");
        fclose(pKeyFile);
        EVP_PKEY_CTX_free(pKeyCtx);
        EVP_PKEY_free(pkey);
        return 1;
    }

    fclose(pKeyFile);
    EVP_PKEY_CTX_free(pKeyCtx);

    // 2. Create a new X509_REQ (CSR)
    X509_REQ *req = X509_REQ_new();
    if (!req)
    {
        fprintf(stderr, "Error creating X509_REQ object\n");
        EVP_PKEY_free(pkey);
        return 1;
    }

    // 3. Set the public key in the CSR
    if (!X509_REQ_set_pubkey(req, pkey))
    {
        fprintf(stderr, "Error setting public key in CSR\n");
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    // 4. Set the subject name in the CSR
    X509_NAME *name = X509_NAME_new();
    if (!name)
    {
        fprintf(stderr, "Error creating X509_NAME object\n");
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    // Add fields to the subject name
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);                           // Country
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char *)"California", -1, -1, 0);                  // State
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char *)"San Francisco", -1, -1, 0);                // Locality
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"My Organization", -1, -1, 0);              // Organization
    X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, (unsigned char *)"My Unit", -1, -1, 0);                     // Organizational Unit
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"www.example.com", -1, -1, 0);             // Common Name
    X509_NAME_add_entry_by_txt(name, "emailAddress", MBSTRING_ASC, (unsigned char *)"user@example.com", -1, -1, 0);  // Email Address

    if (!X509_REQ_set_subject_name(req, name))
    {
        fprintf(stderr, "Error setting subject name in CSR\n");
        X509_NAME_free(name);
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    X509_NAME_free(name);

    // 5. Sign the CSR with the private key
    if (!X509_REQ_sign(req, pkey, EVP_sha256()))
    {
        fprintf(stderr, "Error signing CSR\n");
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    // 6. Write the CSR to a file
    FILE *csr_file = fopen("csr.pem", "w");
    if (!csr_file)
    {
        perror("Error opening CSR file");
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    if (!PEM_write_X509_REQ(csr_file, req))
    {
        fprintf(stderr, "Error writing CSR to file\n");
        fclose(csr_file);
        X509_REQ_free(req);
        EVP_PKEY_free(pkey);
        return 1;
    }

    fclose(csr_file);
    printf("CSR successfully created and written to csr.pem\n");

    // Cleanup
    X509_REQ_free(req);
    EVP_PKEY_free(pkey);

    return 0;
}
