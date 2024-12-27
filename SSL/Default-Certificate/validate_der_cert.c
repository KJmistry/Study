#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <stdio.h>

int main()
{
    const char *cert_file = "cert.der";  // Path to the DER-encoded certificate
    const char *key_file = "key.der";    // Path to the DER-encoded private key
    FILE       *cert_fp = fopen(cert_file, "rb");
    FILE       *key_fp = fopen(key_file, "rb");

    if (!cert_fp)
    {
        fprintf(stderr, "Error opening certificate file: %s\n", cert_file);
        return 1;
    }

    if (!key_fp)
    {
        fprintf(stderr, "Error opening private key file: %s\n", key_file);
        return 1;
    }

    X509 *cert;
    cert = PEM_read_X509(cert_fp, NULL, NULL, NULL);

    fseek(cert_fp, 0, SEEK_SET);  // Reset file pointer to the beginning

    // 1. Load the certificate (DER format)
    cert = d2i_X509_fp(cert_fp, NULL);
    fclose(cert_fp);
    if (!cert)
    {
        fprintf(stderr, "Error loading DER certificate.\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 2. Load the private key (DER format)
    EVP_PKEY *private_key = d2i_PrivateKey_fp(key_fp, NULL);
    fclose(key_fp);
    if (!private_key)
    {
        fprintf(stderr, "Error loading DER private key.\n");
        ERR_print_errors_fp(stderr);
        X509_free(cert);
        return 1;
    }

    // 3. Extract the public key from the certificate
    EVP_PKEY *cert_pubkey = X509_get_pubkey(cert);
    if (!cert_pubkey)
    {
        fprintf(stderr, "Error extracting public key from certificate.\n");
        ERR_print_errors_fp(stderr);
        EVP_PKEY_free(private_key);
        X509_free(cert);
        return 1;
    }

    // 4. Compare the private key's public key with the certificate's public key
    if (EVP_PKEY_cmp(cert_pubkey, private_key) != 1)
    {
        fprintf(stderr, "The private key does not match the certificate's public key.\n");
        EVP_PKEY_free(cert_pubkey);
        EVP_PKEY_free(private_key);
        X509_free(cert);
        return 1;
    }

    printf("The private key matches the certificate's public key.\n");

    // 5. Optionally, verify the certificate's signature (self-signed or signed by CA)
    int ret = X509_verify(cert, private_key);
    if (ret != 1)
    {
        fprintf(stderr, "Certificate signature verification failed: %d\n", ret);
        ERR_print_errors_fp(stderr);
    }
    else
    {
        printf("Certificate signature verification succeeded.\n");
    }

    // Cleanup
    EVP_PKEY_free(cert_pubkey);
    EVP_PKEY_free(private_key);
    X509_free(cert);

    return 0;
}
