// Commands used for key & certificate generation

// openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:prime256v1 -out server_ECDHE.key

// openssl req -x509 -new -key server_ECDHE.key -out server_ECDHE.crt -days 3650 -subj "/C=IN/ST=Gujarat/L=Vadodara/O=Matrix Comsec Pvt. Ltd./OU=R&D
// Software/CN=www.MatrixComsec.com/emailAddress=support@matrixcomsec.com"

/**
 * @file server.c
 * @brief A minimal server-side TLS implementation using ECDHE/DHE with OpenSSL.
 *
 * This program sets up a TLS server using OpenSSL with the ECDHE key exchange mechanism
 * and listens for incoming client connections. It uses the provided server's certificate
 * and private key for secure communication.
 *
 * Compile with:
 * gcc -o server server.c -lssl -lcrypto
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

#define PORT        4433
#define BACKLOG     10
#define KEYLOG_FILE "ssl_keylog_ECDHE.txt"

void handle_error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Keylog callback function to log session keys
void keylog_callback(const SSL *ssl, const char *line)
{
    FILE *keylog_file = fopen(KEYLOG_FILE, "a");
    if (keylog_file)
    {
        fprintf(keylog_file, "%s\n", line);  // Log the session key (HMAC-based key log format)
        fclose(keylog_file);
    }
    else
    {
        perror("Unable to open keylog file");
    }
}

int main()
{
    SSL_CTX           *ctx;
    SSL               *ssl;
    int                sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t          client_len = sizeof(client_addr);

    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Create SSL context
    ctx = SSL_CTX_new(TLS_server_method());
    if (ctx == NULL)
    {
        handle_error("SSL_CTX_new");
    }

    // Load the server's certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, "server_ECDHE.crt", SSL_FILETYPE_PEM) <= 0)
    {
        handle_error("SSL_CTX_use_certificate_file");
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "server_ECDHE.key", SSL_FILETYPE_PEM) <= 0)
    {
        handle_error("SSL_CTX_use_PrivateKey_file");
    }

    // Check if the private key matches the certificate
    if (!SSL_CTX_check_private_key(ctx))
    {
        handle_error("SSL_CTX_check_private_key");
    }

    // Register the keylog callback to log session keys
    SSL_CTX_set_keylog_callback(ctx, keylog_callback);

    // Create a socket and bind to a port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        handle_error("socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        handle_error("bind");
    }

    if (listen(sockfd, BACKLOG) < 0)
    {
        handle_error("listen");
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (clientfd < 0)
    {
        handle_error("accept");
    }

    // Create SSL object for the connection
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clientfd);

    if (SSL_accept(ssl) <= 0)
    {
        handle_error("SSL_accept");
    }

    printf("TLS connection established\n");

    // Communicate with the client
    char buffer[1024];
    int  bytes;
    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer))) > 0)
    {
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
        SSL_write(ssl, "Hello from server", 17);
    }

    // Clean up and close the connection
    SSL_shutdown(ssl);
    close(clientfd);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}
