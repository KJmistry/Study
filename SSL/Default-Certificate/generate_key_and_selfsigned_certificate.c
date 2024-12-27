#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <stdio.h>

void generate_key_and_cert(const char *key_file, const char *cert_file)
{
    EVP_PKEY     *pkey = NULL;
    EVP_PKEY_CTX *pkey_ctx = NULL;

    /* Create a new EVP_PKEY object for the key pair */
    pkey = EVP_PKEY_new();
    if (NULL == pkey)
    {
        fprintf(stderr, "Error creating EVP_PKEY object.\n");
        return;
    }

    /* Create a new context for key generation */
    pkey_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (NULL == pkey_ctx)
    {
        fprintf(stderr, "Error creating EVP_PKEY_CTX object.\n");
        return;
    }

    /* Initialize key generation (no need to set RSA parameters manually) */
    if (EVP_PKEY_keygen_init(pkey_ctx) <= 0)
    {
        fprintf(stderr, "Error initializing key generation.\n");
        return;
    }

    /* Set the RSA key size (2048 bits) */
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pkey_ctx, 2048) <= 0)
    {
        fprintf(stderr, "Error setting RSA key size.\n");
        return;
    }

    /* Generate the RSA key pair */
    if (EVP_PKEY_keygen(pkey_ctx, &pkey) <= 0)
    {
        fprintf(stderr, "Error generating key pair.\n");
        return;
    }

    /* Write the private key to a PEM file */
    FILE *key_file_fp = fopen(key_file, "wb");
    if (!key_file_fp)
    {
        perror("Unable to open private key file for writing");
        return;
    }

    if (PEM_write_PrivateKey(key_file_fp, pkey, NULL, NULL, 0, NULL, NULL) != 1)
    {
        fprintf(stderr, "Error writing private key to file.\n");
        fclose(key_file_fp);
        return;
    }

    fclose(key_file_fp);

    /* Create a new X.509 certificate */
    X509 *x509 = X509_new();
    if (NULL == x509)
    {
        fprintf(stderr, "Error creating X509 certificate.\n");
        return;
    }

    /* Set issuer and subject names (same for self-signed certificate) */
    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"IN", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char *)"Gujarat", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char *)"Vadodara", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"MATRIX COMSEC PVT. LTD", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, (unsigned char *)"RnD", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"www.MatrixComsec.com", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "emailAddress", MBSTRING_ASC, (unsigned char *)"Support@MatrixComsec.com", -1, -1, 0);

    /* Set the subject name */
    X509_set_subject_name(x509, name);

    /* Set the issuer name */
    X509_set_issuer_name(x509, name);

    /* Set the certificate version to 3 (X.509 version 3) */
    if (X509_set_version(x509, 2) != 1)
    {
        fprintf(stderr, "Error setting X.509 version.\n");
        return;
    }

    ASN1_INTEGER *serial = X509_get_serialNumber(x509);
    BIGNUM       *bn = BN_new();

    /* Generate a 63-bit random number. The most significant bit (MSB) is set to 0
    to ensure the number is non-negative. This avoids issues with ASN.1 encoding,
    where leading zeros might be added in some cases when the number is treated as an ASN.1 INTEGER.*/
    BN_rand(bn, 63, 0, 0);
    BN_to_ASN1_INTEGER(bn, serial);

    /* Free resources */
    BN_free(bn);
    ASN1_INTEGER_free(serial);

    /* Set the serial number in the X509 certificate */
    X509_set_serialNumber(x509, serial);

    /* Set the validity period (10 years) */
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), (60 * 60 * 24 * 365 * 10));  // 10 years

    /* Set the public key in the certificate */
    if (X509_set_pubkey(x509, pkey) != 1)
    {
        fprintf(stderr, "Error setting public key.\n");
        return;
    }

    X509_EXTENSION *ext;
    X509V3_CTX      ctx;

    // /* Add the Subject Key Identifier extension using the X509V3 extension API */
    // X509V3_set_ctx(&ctx, x509, x509, NULL, NULL, 0);

    // ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_key_identifier, "hash");
    // if (ext == NULL)
    // {
    //     fprintf(stderr, "Error adding Subject Key Identifier extension\n");
    //     return;
    // }

    // if (X509_add_ext(x509, ext, -1) != 1)
    // {
    //     fprintf(stderr, "Error adding Subject Key Identifier extension to certificate\n");
    //     X509_EXTENSION_free(ext);
    //     return;
    // }

    // X509_EXTENSION_free(ext);

    /* Add the Subject Alternative Name (SAN) extension */
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_alt_name, "DNS:www.MatrixComsec.com");
    if (ext == NULL)
    {
        fprintf(stderr, "Error creating Subject Alternative Name extension.\n");
        return;
    }

    if (X509_add_ext(x509, ext, -1) != 1)
    {
        fprintf(stderr, "Error adding Subject Alternative Name extension to certificate.\n");
        X509_EXTENSION_free(ext);
        return;
    }

    X509_EXTENSION_free(ext);

    /* Add the Key Usage extension */
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_key_usage, "digitalSignature,keyEncipherment");
    if (ext == NULL)
    {
        fprintf(stderr, "Error creating Key Usage extension.\n");
        return;
    }

    /* Mark the extension as critical */
    X509_EXTENSION_set_critical(ext, 1);

    /* Add the Key Usage extension to the certificate */
    if (X509_add_ext(x509, ext, -1) != 1)
    {
        fprintf(stderr, "Error adding Key Usage extension to certificate.\n");
        X509_EXTENSION_free(ext);
        return;
    }

    X509_EXTENSION_free(ext);

    /* Add the Extended Key Usage extension */
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_ext_key_usage, "serverAuth,clientAuth");
    if (ext == NULL)
    {
        fprintf(stderr, "Error creating Extended Key Usage extension.\n");
        return;
    }

    /* Mark the extension as critical */
    X509_EXTENSION_set_critical(ext, 1);

    /* Add the Extended Key Usage extension to the certificate */
    if (X509_add_ext(x509, ext, -1) != 1)
    {
        fprintf(stderr, "Error adding Extended Key Usage extension to certificate.\n");
        X509_EXTENSION_free(ext);
        return;
    }

    X509_EXTENSION_free(ext);

    // /* Configure Extension field parameters

    // Identifier: 2.5.29.19
    // Breakdown :
    // 2: ISO/ITU-T standard.
    // 5: Directory services (e.g., X.500)
    // 29: Specifies extensions under X.509
    // 19: Basic Constraints extension */

    // ASN1_OCTET_STRING *basic_constraints_data = ASN1_OCTET_STRING_new();
    // if (NULL == basic_constraints_data)
    // {
    //     fprintf(stderr, "Error creating ASN1_OCTET_STRING for basicConstraints\n");
    //     return;
    // }

    // /* We are setting the value "CA:TRUE" to indicate that this certificate is a Certificate Authority (CA) */
    // if (0 == ASN1_OCTET_STRING_set(basic_constraints_data, (unsigned char *)"CA:TRUE", 24))
    // {
    //     fprintf(stderr, "Error setting ASN1_OCTET_STRING data\n");
    //     ASN1_OCTET_STRING_free(basic_constraints_data);
    //     return;
    // }

    // /* The '0' indicates that this extension is non-critical */
    // ext = X509_EXTENSION_create_by_NID(NULL, NID_basic_constraints, 0, basic_constraints_data);
    // if (!ext)
    // {
    //     fprintf(stderr, "Error creating X509 extension\n");
    //     ASN1_OCTET_STRING_free(basic_constraints_data);
    //     return;
    // }

    // /* Add the extension to the certificate (x509) */
    // X509_add_ext(x509, ext, -1);

    // /* Free resources */
    // X509_EXTENSION_free(ext);
    // ASN1_OCTET_STRING_free(basic_constraints_data);

    /* Set the basicConstraints extension to mark it as a CA, with no path length specified (i.e., unlimited path length) */
    ext = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, "CA:FALSE");

    /* Mark the extension as critical */
    // X509_EXTENSION_set_critical(ext, 1);

    /* Add the Basic Constraints extension to the certificate */
    if (0 == X509_add_ext(x509, ext, -1))
    {
        fprintf(stderr, "Error adding Basic Constraints extension\n");
        X509_EXTENSION_free(ext);
        return;
    }

    X509_EXTENSION_free(ext);

    /* Sign the certificate using the private key (self-signed) */
    if (X509_sign(x509, pkey, EVP_sha256()) == 0)
    {
        fprintf(stderr, "Error signing the certificate.\n");
        return;
    }

    /* Write the certificate to a PEM file */
    FILE *cert_file_fp = fopen(cert_file, "wb");
    if (NULL == cert_file_fp)
    {
        perror("Unable to open certificate file for writing");
        return;
    }

    if (PEM_write_X509(cert_file_fp, x509) != 1)
    {
        fclose(cert_file_fp);
        fprintf(stderr, "Error writing certificate to file.\n");
        return;
    }

    fclose(cert_file_fp);

    /* Cleanup */
    EVP_PKEY_free(pkey);
    X509_free(x509);
    EVP_PKEY_CTX_free(pkey_ctx);
}

int main()
{
    const char *key_file = "server.key";
    const char *cert_file = "server.crt";

    /* Generate RSA private key and X.509 certificate */
    generate_key_and_cert(key_file, cert_file);

    printf("Private key and certificate generated successfully.\n");
    return 0;
}
