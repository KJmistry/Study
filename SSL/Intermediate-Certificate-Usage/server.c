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

// Port for SSL server
#define PORT 4433

// Function to initialize OpenSSL library
void initialize_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

// Function to clean up OpenSSL
void cleanup_openssl()
{
    EVP_cleanup();
}

// Function to create an SSL context
SSL_CTX* create_context()
{
    const SSL_METHOD* method = TLS_server_method();
    SSL_CTX*          ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

// Function to load certificates into the SSL context
void configure_context(SSL_CTX* ctx)
{
    // Load the server certificate
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Load the private key
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Load the intermediate certificate chain
    FILE* intermediate_file = fopen("intermediate.crt", "r");
    if (intermediate_file == NULL)
    {
        perror("Unable to open intermediate.crt");
        exit(EXIT_FAILURE);
    }

    // Read intermediate certificate into X509 structure
    X509* intermediate_cert = PEM_read_X509(intermediate_file, NULL, NULL, NULL);
    fclose(intermediate_file);

    if (intermediate_cert == NULL)
    {
        fprintf(stderr, "Unable to load intermediate certificate\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Add the intermediate certificate to the SSL context
    if (SSL_CTX_add0_chain_cert(ctx, intermediate_cert) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Verify the private key matches the certificate
    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        exit(EXIT_FAILURE);
    }
}

// Function to handle SSL connection
void handle_connection(SSL* ssl)
{
    char buf[1024];
    int  bytes;

    // SSL handshake
    if (SSL_accept(ssl) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return;
    }

    // Read data from the client
    bytes = SSL_read(ssl, buf, sizeof(buf));
    if (bytes > 0)
    {
        buf[bytes] = '\0';
        printf("Received message: %s\n", buf);
    }
    else
    {
        ERR_print_errors_fp(stderr);
    }

    // Send a response to the client
    const char* msg = "Hello from the SSL server!";
    SSL_write(ssl, msg, strlen(msg));
}

int main()
{
    // Initialize OpenSSL
    initialize_openssl();

    // Create an SSL context
    SSL_CTX* ctx = create_context();

    // Configure the context with certificates
    configure_context(ctx);

    // Create and configure the server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Unable to create server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to bind server socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) < 0)
    {
        perror("Unable to listen on server socket");
        exit(EXIT_FAILURE);
    }

    printf("SSL server is running on port %d\n", PORT);

    while (1)
    {
        int client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0)
        {
            perror("Unable to accept client connection");
            continue;
        }

        // Create SSL object for the connection
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);

        // Handle the SSL connection
        handle_connection(ssl);

        // Clean up the SSL object and client connection
        SSL_free(ssl);
        close(client_sock);
    }

    close(server_sock);
    SSL_CTX_free(ctx);

    // Clean up OpenSSL
    cleanup_openssl();

    return 0;
}
