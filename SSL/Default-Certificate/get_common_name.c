#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <stdio.h>
#include <stdlib.h>

char *get_common_name_from_cert(const char *cert_path)
{
    FILE      *cert_file = NULL;
    X509      *cert = NULL;
    X509_NAME *subject_name = NULL;
    char       common_name[50];
    uint8_t   *rawData = NULL;
    int        len = 0;

    // Open the certificate file
    cert_file = fopen(cert_path, "r");
    if (!cert_file)
    {
        perror("Error opening certificate file");
        return NULL;
    }

    // Read the certificate
    cert = PEM_read_X509(cert_file, NULL, NULL, NULL);
    fclose(cert_file);

    if (!cert)
    {
        fprintf(stderr, "Error reading certificate: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return NULL;
    }

    // Get the subject name
    subject_name = X509_get_subject_name(cert);
    if (!subject_name)
    {
        fprintf(stderr, "Error getting subject name: %s\n", ERR_error_string(ERR_get_error(), NULL));
        X509_free(cert);
        return NULL;
    }

    // Extract the common name
    int index = X509_NAME_get_index_by_NID(subject_name, NID_commonName, -1);
    if (index < 0)
    {
        fprintf(stderr, "Common Name not found in certificate\n");
        X509_free(cert);
        return NULL;
    }

    X509_NAME_ENTRY *entry = X509_NAME_get_entry(subject_name, index);
    ASN1_STRING     *data = X509_NAME_ENTRY_get_data(entry);

    // Convert ASN1_STRING to a C string
    rawData = (uint8_t *)ASN1_STRING_get0_data(data);
    len = ASN1_STRING_length(data);
    // Ensure the string is null-terminated
    // if (ASN1_STRING_length(data) != (int)strlen(common_name))
    // {
    //     fprintf(stderr, "Common Name contains embedded nulls\n");
    //     X509_free(cert);
    //     return NULL;
    // }

    snprintf(common_name, 50, "%.*s", len, rawData);
    // Duplicate the string to return (since it's part of the cert's memory)
    char *cn_copy = strdup(common_name);
    X509_free(cert);

    return cn_copy;  // Caller must free this
}

int main()
{
    const char *cert_path = "server.crt";

    char *common_name = get_common_name_from_cert(cert_path);
    if (common_name)
    {
        printf("Common Name: %s\n", common_name);
        free(common_name);
    }
    else
    {
        fprintf(stderr, "Failed to extract Common Name.\n");
    }

    return 0;
}
