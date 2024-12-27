#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PORT        12345
#define CERT_FILE   "server.crt"
#define KEY_FILE    "server.key"
#define LOG_FILE    "ssl_server_log.txt"
#define KEYLOG_FILE "ssl_keylog.txt"

// Commands used for key and certificate generation

// openssl genpkey -algorithm RSA -out server.key

// openssl req -x509 -new -key server.key -out server.crt -days 3650 -subj "/C=IN/ST=Gujarat/L=Vadodara/O=Matrix Comsec Pvt.
// Ltd./OU=R&DSoftware/CN=www.MatrixComsec.com/emailAddress=support@matrixcomsec.com"

// Function to initialize OpenSSL

void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

// Function to clean up OpenSSL
void cleanup_openssl()
{
    EVP_cleanup();
}

// Function to create server SSL context
SSL_CTX* create_server_context()
{
    const SSL_METHOD* method;
    SSL_CTX*          ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

// Function to configure server context with certificate and private key
void configure_server_context(SSL_CTX* ctx)
{
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        perror("Unable to load certificate");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        perror("Unable to load private key");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_check_private_key(ctx) == 0)
    {
        perror("Private key does not match certificate");
        ERR_print_errors_fp(stderr);
        exit(1);
    }
}

// Keylog callback function to log session keys
void keylog_callback(const SSL* ssl, const char* line)
{
    FILE* keylog_file = fopen(KEYLOG_FILE, "a");
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

// Function to log server-side SSL connection details to a file
void log_ssl_details(SSL* ssl, const char* client_ip)
{
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL)
    {
        perror("Unable to open log file");
        return;
    }

    // Get the current time
    time_t     current_time = time(NULL);
    struct tm* tm_info = localtime(&current_time);
    char       time_buffer[26];
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    // Log the connection details
    fprintf(log_file, "Time: %s\n", time_buffer);
    fprintf(log_file, "Client IP: %s\n", client_ip);
    fprintf(log_file, "SSL/TLS Version: %s\n", SSL_get_version(ssl));
    fprintf(log_file, "Cipher Suite: %s\n", SSL_get_cipher(ssl));

    // You can also log more details if needed, like client certificate (if any)
    if (SSL_get_peer_certificate(ssl) != NULL)
    {
        fprintf(log_file, "Client Certificate: Present\n");
    }
    else
    {
        fprintf(log_file, "Client Certificate: None\n");
    }

    fprintf(log_file, "----------------------------------------\n");

    fclose(log_file);
}

int main()
{
    int                server_fd, client_fd;
    SSL_CTX*           ctx;
    SSL*               ssl;
    struct sockaddr_in addr;
    char               buffer[1024];

    init_openssl();
    ctx = create_server_context();
    configure_server_context(ctx);

    // Register the keylog callback to log session keys
    SSL_CTX_set_keylog_callback(ctx, keylog_callback);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Unable to create socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to bind socket");
        exit(1);
    }

    if (listen(server_fd, 1) < 0)
    {
        perror("Unable to listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t          client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("Unable to accept connection");
        exit(1);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);

    if (SSL_accept(ssl) <= 0)
    {
        perror("SSL accept failed");
        ERR_print_errors_fp(stderr);
    }
    else
    {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        printf("SSL connection established with %s\n", client_ip);

        // Log the SSL connection details
        log_ssl_details(ssl, client_ip);

        // Send data to client
        SSL_write(ssl, "Hello from secure server!\n", 25);

        // Receive data from client
        int bytes = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes > 0)
        {
            buffer[bytes] = 0;
            printf("Received from client: %s\n", buffer);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_fd);
    close(server_fd);
    SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}
