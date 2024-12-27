#include <arpa/inet.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RTSP_PORT   554  // Default RTSP port
#define BUFFER_SIZE 8192

// Function to calculate the MD5 hash
void md5(const char *str, unsigned char *output)
{
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, str, strlen(str));
    MD5_Final(output, &md5_ctx);
}

// Convert bytes to hex string
void bytes_to_hex(const unsigned char *bytes, char *hex_str, int length)
{
    for (int i = 0; i < length; i++)
    {
        sprintf(hex_str + i * 2, "%02x", bytes[i]);
    }
    hex_str[length * 2] = '\0';
}

// Function to send RTSP DESCRIBE request with Digest Authentication
void send_rtsp_describe_request(const char *server_ip, const char *rtsp_url, const char *username, const char *password, const char *nonce)
{
    int                sock;
    struct sockaddr_in server_addr;
    char               request[BUFFER_SIZE];
    char               response[BUFFER_SIZE];
    char               auth_header[BUFFER_SIZE];
    unsigned char      hash1[MD5_DIGEST_LENGTH], hash2[MD5_DIGEST_LENGTH];
    char               hash1_hex[MD5_DIGEST_LENGTH * 2 + 1], hash2_hex[MD5_DIGEST_LENGTH * 2 + 1];
    char               response_hash[MD5_DIGEST_LENGTH * 2 + 1];

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(RTSP_PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the RTSP server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Format the RTSP DESCRIBE request
    snprintf(request, sizeof(request),
             "DESCRIBE %s RTSP/1.0\r\n"
             "CSeq: 1\r\n"
             "User-Agent: libRTSP (LIVE555 Streaming Media v2024.05.30)\r\n"  // Optional ** copied from pcap
             "Accept: application/sdp\r\n"
             "\r\n",
             rtsp_url);

    // Send the DESCRIBE request
    if (send(sock, request, strlen(request), 0) < 0)
    {
        perror("Send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Receive the response
    int recv_len = recv(sock, response, sizeof(response) - 1, 0);
    if (recv_len < 0)
    {
        perror("Receive failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the response
    response[recv_len] = '\0';

    // Check for 401 Unauthorized response
    if (strstr(response, "401 Unauthorized"))
    {
        // Extract the nonce value from the response (you may need to parse this more robustly)
        char *nonce_start = strstr(response, "nonce=\"");
        if (nonce_start)
        {
            nonce_start += 7;  // Skip over "nonce=\""
            char *nonce_end = strstr(nonce_start, "\"");
            if (nonce_end)
            {
                *nonce_end = '\0';  // Null-terminate the nonce string
                printf("Nonce: %s\n", nonce_start);

                // Compute Digest response (using MD5)
                // First, hash(username:realm:password)
                char hash1_str[BUFFER_SIZE];
                snprintf(hash1_str, sizeof(hash1_str), "%s:%s:%s", username, "LIVE555 Streaming Media", password);
                md5(hash1_str, hash1);
                bytes_to_hex(hash1, hash1_hex, MD5_DIGEST_LENGTH);

                // Then, hash(DESCRIBE:uri)
                md5("DESCRIBE:rtsp://192.168.101.47:554/unicaststream/2", hash2);
                bytes_to_hex(hash2, hash2_hex, MD5_DIGEST_LENGTH);

                // Finally, concatenate and hash to get the response
                char final_str[BUFFER_SIZE];
                snprintf(final_str, sizeof(final_str), "%s:%s:%s", hash1_hex, nonce_start, hash2_hex);
                md5(final_str, hash2);
                bytes_to_hex(hash2, response_hash, MD5_DIGEST_LENGTH);

                // Create the Authorization header
                snprintf(auth_header, sizeof(auth_header),
                         "Authorization: Digest username=\"%s\", realm=\"LIVE555 Streaming Media\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
                         username, nonce_start, rtsp_url, response_hash);

                // Send the second DESCRIBE request with Digest Authentication
                snprintf(request, sizeof(request),
                         "DESCRIBE %s RTSP/1.0\r\n"
                         "CSeq: 2\r\n"
                         "User-Agent: libRTSP (LIVE555 Streaming Media v2024.05.30)\r\n"
                         "Accept: application/sdp\r\n"
                         "%s\r\n",
                         rtsp_url, auth_header);

                // Send the authenticated DESCRIBE request
                send(sock, request, strlen(request), 0);

                // Receive and print the response
                recv_len = recv(sock, response, sizeof(response) - 1, 0);
                response[recv_len] = '\0';
                printf("Response:\n%s\n", response);
            }
        }
    }

    // Close the socket
    close(sock);
}

int main()
{
    const char *server_ip = "192.168.101.47";
    const char *rtsp_url = "rtsp://192.168.101.47:554/unicaststream/2";
    const char *username = "admin";  // Replace with actual username if different
    const char *password = "admin";  // Replace with actual password if different
    const char *nonce = "";          // This will be extracted from the response

    send_rtsp_describe_request(server_ip, rtsp_url, username, password, nonce);

    return 0;
}
