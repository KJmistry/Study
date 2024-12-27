#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER "127.0.0.1"
#define PORT   4433

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

    method = TLSv1_2_client_method();  // Use TLS 1.2 only
    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

// Load server certificates
void configure_context(SSL_CTX *ctx)
{
    if (SSL_CTX_load_verify_locations(ctx, "server.crt", NULL) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(1);
    }
}

int main()
{
    int                sock;
    struct sockaddr_in addr;
    SSL_CTX           *ctx;
    SSL               *ssl;
    char               buf[1024];
    char              *server_response = "Server response:\n";

    init_openssl();
    ctx = create_context();
    configure_context(ctx);

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Unable to create socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to connect");
        exit(1);
    }

    // Create SSL object
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    // Perform TLS handshake (only TLS 1.2 allowed)
    if (SSL_connect(ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
        printf("TLS handshake successful. Receiving response...\n");

        // Read server's response
        int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);
        buf[bytes] = 0;
        printf("%s", buf);
    }

    // Clean up
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    return 0;
}
