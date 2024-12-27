#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_cert_and_key(const char *p12_file, const char *password)
{
    FILE     *fp = NULL;
    PKCS12   *p12 = NULL;
    EVP_PKEY *private_key = NULL;
    X509     *certificate = NULL;
    STACK_OF(X509) *cert_stack = NULL;
    int result;

    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    // ERR_load_crypto_strings();
    OpenSSL_add_all_ciphers();

    // Open the .p12 file
    fp = fopen(p12_file, "rb");
    if (!fp)
    {
        perror("Error opening file");
        return;
    }

    // Read the .p12 file into PKCS#12 structure
    p12 = d2i_PKCS12_fp(fp, NULL);
    fclose(fp);

    if (!p12)
    {
        fprintf(stderr, "Error reading PKCS#12 file\n");
        return;
    }

    // Parse the PKCS#12 structure with the password
    result = PKCS12_parse(p12, password, &private_key, &certificate, &cert_stack);
    if (!result)
    {
        fprintf(stderr, "Error parsing PKCS#12 file with the given password\n");
        PKCS12_free(p12);
        return;
    }

    // Extract and save the private key
    fp = fopen("output/private_key.pem", "wb");
    if (fp)
    {
        PEM_write_PrivateKey(fp, private_key, NULL, NULL, 0, NULL, NULL);
        fclose(fp);
        printf("Private key extracted and saved to private_key.pem\n");
    }
    else
    {
        fprintf(stderr, "Error saving private key\n");
    }

    // Extract and save the certificate
    fp = fopen("output/certificate.pem", "wb");
    if (fp)
    {
        PEM_write_X509(fp, certificate);
        fclose(fp);
        printf("Certificate extracted and saved to certificate.pem\n");
    }
    else
    {
        fprintf(stderr, "Error saving certificate\n");
    }

    // Cleanup
    EVP_PKEY_free(private_key);
    X509_free(certificate);
    PKCS12_free(p12);
    sk_X509_free(cert_stack);
}

int main()
{
    const char *p12_file = "certificate.p12";  // Replace with your .p12 file path
    const char *password = "kshitij";          // Replace with the password for the .p12 file

    extract_cert_and_key(p12_file, password);
    return 0;
}
