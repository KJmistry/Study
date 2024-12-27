#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

#define AES_KEY_SIZE 32  // 256 bits
#define AES_IV_SIZE  16  // 128 bits
#define AES_TAG_SIZE 16  // GCM tag size

// Define the key and IV as hex strings
#define KEY          "f581bc828c9b63c964bd45ba07538920a1df79c2b533b42c8f744e2993887573"
#define IV           "2dd1b759af912e1294037696296a4148"

// Function to convert a hex string to a byte array
int hex_to_bytes(const char *hex_str, unsigned char *output)
{
    size_t len = strlen(hex_str);
    if (len % 2 != 0)
    {
        // Hex string length must be even
        return -1;
    }

    for (size_t i = 0; i < len / 2; i++)
    {
        sscanf(hex_str + 2 * i, "%2hhx", &output[i]);
    }

    return len / 2;
}

// AES 256 GCM encryption function
// Encrypts the provided plaintext using AES-256-GCM with the provided key and IV.
int aes_256_gcm_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext,
                        unsigned char *tag)
{
    EVP_CIPHER_CTX *ctx;
    int             len, ciphertext_len;

    // Create and initialize the context for the encryption operation
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        return 0;  // Return 0 if context creation fails
    }

    // Initialize the encryption operation using AES-256-GCM mode
    // EVP_aes_256_gcm() tells OpenSSL to use the AES algorithm in GCM mode
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context in case of failure
        return 0;
    }

    // Set the length of the initialization vector (IV) (AES_IV_SIZE is a constant)
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_IV_SIZE, NULL))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context if setting IV length fails
        return 0;
    }

    // Initialize the encryption key and IV for the AES encryption
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context if key/IV initialization fails
        return 0;
    }

    // Perform the encryption on the plaintext, writing the ciphertext to the output buffer
    // len is the size of the encrypted data processed so far
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context if encryption update fails
        return 0;
    }

    // Store the total length of the ciphertext produced so far
    ciphertext_len = len;

    // Finalize the encryption, adding any padding if necessary, and update the ciphertext
    // len will store the final size of the ciphertext produced during finalization
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context if finalization fails
        return 0;
    }

    // Add the length of the final block to the total ciphertext length
    ciphertext_len += len;

    // Retrieve the authentication tag for the GCM mode
    // GCM mode requires the authentication tag to ensure integrity
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_TAG_SIZE, tag))
    {
        EVP_CIPHER_CTX_free(ctx);  // Free context if tag retrieval fails
        return 0;
    }

    // Free the context to clean up the resources
    EVP_CIPHER_CTX_free(ctx);

    // Return the total length of the ciphertext
    return ciphertext_len;
}

// Function to encode in Base64
void base64_encode(unsigned char *input, int length, char *output)
{
    BIO     *bio, *b64;
    BUF_MEM *bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    memcpy(output, bufferPtr->data, bufferPtr->length);
    output[bufferPtr->length] = '\0';
    BIO_free_all(bio);
}

int main()
{
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];
    unsigned char plaintext[] = "admin";
    unsigned char ciphertext[1024];
    unsigned char tag[AES_TAG_SIZE];
    int           ciphertext_len;

    // Convert the KEY and IV macros to byte arrays
    if (hex_to_bytes(KEY, key) < 0)
    {
        printf("Invalid key format\n");
        return 1;
    }
    if (hex_to_bytes(IV, iv) < 0)
    {
        printf("Invalid IV format\n");
        return 1;
    }

    // Perform AES encryption
    ciphertext_len = aes_256_gcm_encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext, tag);

    if (ciphertext_len == 0)
    {
        printf("Encryption failed\n");
        return 1;
    }

    // Concatenate the ciphertext and the tag
    unsigned char ciphertext_with_tag[1024];
    memcpy(ciphertext_with_tag, ciphertext, ciphertext_len);
    memcpy(ciphertext_with_tag + ciphertext_len, tag, AES_TAG_SIZE);

    // Print ciphertext + tag in Base64
    char base64_output[2048];
    base64_encode(ciphertext_with_tag, ciphertext_len + AES_TAG_SIZE, base64_output);

    printf("Encrypted (Base64): %s\n", base64_output);

    return 0;
}
