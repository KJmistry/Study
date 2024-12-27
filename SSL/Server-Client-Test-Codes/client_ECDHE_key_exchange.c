/**
 * @file client.c
 * @brief A minimal client-side TLS implementation using ECDHE/DHE with OpenSSL.
 *
 * This program connects to a TLS server using OpenSSL with the ECDHE key exchange mechanism.
 * It uses the server's public certificate to verify the server identity and establish a secure connection.
 *
 * Compile with:
 * gcc -o client client.c -lssl -lcrypto
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER "127.0.0.1"
#define PORT   4433

void handle_error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    SSL_CTX           *ctx;
    SSL               *ssl;
    int                sockfd;
    struct sockaddr_in server_addr;

    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Create SSL context
    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL)
    {
        handle_error("SSL_CTX_new");
    }

    // Load the server's certificate to verify its identity
    if (SSL_CTX_load_verify_locations(ctx, "server_ECDHE.crt", NULL) <= 0)
    {
        handle_error("SSL_CTX_load_verify_locations");
    }

    // Create a socket and connect to the server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        handle_error("socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER);
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        handle_error("connect");
    }

    // Create SSL object for the connection
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) <= 0)
    {
        handle_error("SSL_connect");
    }

    printf("TLS connection established\n");

    // Communicate with the server
    SSL_write(ssl, "Hello from client", 17);
    char buffer[1024];
    int  bytes = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
    }

    // Clean up and close the connection
    SSL_shutdown(ssl);
    close(sockfd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}
