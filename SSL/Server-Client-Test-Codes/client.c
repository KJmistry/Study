#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 12345
#define CERT_FILE   "server.crt"

void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX* create_client_context()
{
    const SSL_METHOD* method;
    SSL_CTX*          ctx;

    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

int main()
{
    int                sock;
    struct sockaddr_in server_addr;
    SSL_CTX*           ctx;
    SSL*               ssl;
    char               buffer[1024];

    init_openssl();
    ctx = create_client_context();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Unable to create socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        perror("Unable to connect to server");
        exit(1);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0)
    {
        perror("SSL connect failed");
        ERR_print_errors_fp(stderr);
    }
    else
    {
        printf("SSL connection established\n");

        // Receive data from the server
        int bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes > 0)
        {
            buffer[bytes] = 0;
            printf("Received from server: %s\n", buffer);
        }

        // Send data to server
        SSL_write(ssl, "Hello, server! This is a secure client.\n", 39);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}
