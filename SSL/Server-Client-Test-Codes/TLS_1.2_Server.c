#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 4433

// Initialize OpenSSL library
void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

// Cleanup OpenSSL library
void cleanup_openssl()
{
    EVP_cleanup();
}

// Create a new SSL context for TLS 1.2
SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX          *ctx;

    method = TLSv1_2_server_method();  // Use TLS 1.2 only
    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

// Load server certificates and private key
void configure_context(SSL_CTX *ctx)
{
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    // Ensure the private key matches the certificate
    if (!SSL_CTX_check_private_key(ctx))
    {
        perror("Private key does not match the certificate");
        exit(1);
    }
}

int main()
{
    int                server_fd, client_fd;
    struct sockaddr_in addr;
    SSL_CTX           *ctx;
    SSL               *ssl;
    char              *response = "Hello, TLS 1.2 Client!\n";

    init_openssl();
    ctx = create_context();
    configure_context(ctx);

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Unable to create socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to bind");
        exit(1);
    }

    if (listen(server_fd, 1) < 0)
    {
        perror("Unable to listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming client connection
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("Unable to accept client connection");
        exit(1);
    }

    // Create SSL object
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    // Perform TLS handshake (only TLS 1.2 allowed)
    if (SSL_accept(ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
        printf("TLS handshake successful. Sending response...\n");

        // Send data to the client
        SSL_write(ssl, response, strlen(response));
    }

    // Clean up
    SSL_free(ssl);
    close(client_fd);
    close(server_fd);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    return 0;
}
