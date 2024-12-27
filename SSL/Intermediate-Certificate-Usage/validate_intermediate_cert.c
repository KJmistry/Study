#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define SUCCESS   1
#define FAIL      0
#define STATUS_OK 0

static int validateIntermediateCertificate(const char *intermediateCertPath, const char *signedCertPath)
{
    FILE           *intermediateFile = NULL;
    FILE           *signedFile = NULL;
    X509           *intermediateCert = NULL;
    X509           *signedCert = NULL;
    X509_STORE     *store = NULL;
    X509_STORE_CTX *ctx = NULL;
    int             status = FAIL;

    do
    {
        // Check if intermediate certificate file exists
        if (access(intermediateCertPath, F_OK) != STATUS_OK)
        {
            fprintf(stderr, "Intermediate certificate file does not exist: [path=%s]\n", intermediateCertPath);
            break;
        }

        // Open and read the intermediate certificate
        intermediateFile = fopen(intermediateCertPath, "rb");
        if (!intermediateFile)
        {
            fprintf(stderr, "Unable to open intermediate certificate file: [path=%s]\n", intermediateCertPath);
            break;
        }

        intermediateCert = PEM_read_X509(intermediateFile, NULL, NULL, NULL);
        fclose(intermediateFile);

        if (!intermediateCert)
        {
            fprintf(stderr, "Failed to parse intermediate certificate: [path=%s]\n", intermediateCertPath);
            break;
        }

        // Check if signed certificate file exists
        if (access(signedCertPath, F_OK) != STATUS_OK)
        {
            fprintf(stderr, "Signed certificate file does not exist: [path=%s]\n", signedCertPath);
            break;
        }

        // Open and read the signed certificate
        signedFile = fopen(signedCertPath, "rb");
        if (!signedFile)
        {
            fprintf(stderr, "Unable to open signed certificate file: [path=%s]\n", signedCertPath);
            break;
        }

        signedCert = PEM_read_X509(signedFile, NULL, NULL, NULL);
        fclose(signedFile);

        if (!signedCert)
        {
            fprintf(stderr, "Failed to parse signed certificate: [path=%s]\n", signedCertPath);
            break;
        }

        // Create a certificate store and add the signed certificate
        store = X509_STORE_new();
        if (!store)
        {
            fprintf(stderr, "Failed to create X509_STORE\n");
            break;
        }

        if (X509_STORE_add_cert(store, signedCert) != 1)
        {
            fprintf(stderr, "Failed to add signed certificate to X509_STORE\n");
            break;
        }

        // Create a certificate store context for validation
        ctx = X509_STORE_CTX_new();
        if (!ctx)
        {
            fprintf(stderr, "Failed to create X509_STORE_CTX\n");
            break;
        }

        if (X509_STORE_CTX_init(ctx, store, intermediateCert, NULL) != 1)
        {
            fprintf(stderr, "Failed to initialize X509_STORE_CTX\n");
            break;
        }

        // Perform certificate validation
        if (X509_verify_cert(ctx) != 1)
        {
            fprintf(stderr, "Certificate validation failed: %s\n", X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx)));
            break;
        }

        // Certificate is valid
        fprintf(stdout, "Intermediate certificate is valid.\n");
        status = SUCCESS;

    } while (0);

    // Cleanup
    if (ctx)
    {
        X509_STORE_CTX_free(ctx);
    }
    if (store)
    {
        X509_STORE_free(store);
    }
    if (signedCert)
    {
        X509_free(signedCert);
    }
    if (intermediateCert)
    {
        X509_free(intermediateCert);
    }

    return status;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IntermediateCertPath> <SignedCertPath>\n", argv[0]);
        return 1;
    }

    const char *intermediateCertPath = argv[1];
    const char *signedCertPath = argv[2];

    if (validateIntermediateCertificate(intermediateCertPath, signedCertPath) == SUCCESS)
    {
        printf("Intermediate certificate validation succeeded.\n");
    }
    else
    {
        printf("Intermediate certificate validation failed.\n");
    }

    return 0;
}
