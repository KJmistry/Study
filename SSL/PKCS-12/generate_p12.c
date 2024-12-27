#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/x509.h>
#include <stdio.h>

void generate_p12_file(const char *cert_file, const char *key_file, const char *p12_file, const char *password)
{
    FILE     *cert_fp = NULL;
    FILE     *key_fp = NULL;
    X509     *cert = NULL;
    EVP_PKEY *private_key = NULL;
    PKCS12   *p12 = NULL;
    FILE     *p12_fp = NULL;

    // Open certificate file
    cert_fp = fopen(cert_file, "r");
    if (!cert_fp)
    {
        fprintf(stderr, "Error opening certificate file: %s\n", cert_file);
        return;
    }

    // Load certificate from file
    cert = PEM_read_X509(cert_fp, NULL, NULL, NULL);
    fclose(cert_fp);
    if (!cert)
    {
        fprintf(stderr, "Error loading certificate from file\n");
        return;
    }

    // Open private key file
    key_fp = fopen(key_file, "r");
    if (!key_fp)
    {
        fprintf(stderr, "Error opening private key file: %s\n", key_file);
        X509_free(cert);
        return;
    }

    // Load private key from file
    private_key = PEM_read_PrivateKey(key_fp, NULL, NULL, NULL);
    fclose(key_fp);
    if (!private_key)
    {
        fprintf(stderr, "Error loading private key from file\n");
        X509_free(cert);
        return;
    }

    // Create PKCS12 structure
    p12 = PKCS12_create((char *)password, NULL, private_key, cert, NULL, 0, 0, 0, 0, 0);
    if (!p12)
    {
        fprintf(stderr, "Error creating PKCS12 structure\n");
        EVP_PKEY_free(private_key);
        X509_free(cert);
        return;
    }

    // Open output p12 file
    p12_fp = fopen(p12_file, "wb");
    if (!p12_fp)
    {
        fprintf(stderr, "Error opening p12 file for writing: %s\n", p12_file);
        PKCS12_free(p12);
        EVP_PKEY_free(private_key);
        X509_free(cert);
        return;
    }

    // Write PKCS12 structure to the file
    if (!i2d_PKCS12_fp(p12_fp, p12))
    {
        fprintf(stderr, "Error writing PKCS12 structure to file\n");
    }
    else
    {
        printf("PKCS12 file generated successfully: %s\n", p12_file);
    }

    // Clean up
    fclose(p12_fp);
    PKCS12_free(p12);
    EVP_PKEY_free(private_key);
    X509_free(cert);
}

int main()
{
    const char *cert_file = "server.crt";  // Path to your certificate file
    const char *key_file = "server.key";   // Path to your private key file
    const char *p12_file = "output.p12";   // Path to output PKCS#12 file
    const char *password = "";             // Password to protect the .p12 file

    // Initialize OpenSSL (required for certain versions)
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Generate the p12 file
    generate_p12_file(cert_file, key_file, p12_file, password);

    // Clean up OpenSSL (required for certain versions)
    ERR_free_strings();
    EVP_cleanup();

    return 0;
}
