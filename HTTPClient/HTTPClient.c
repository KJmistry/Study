#include <ctype.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URL_LENGTH  1024
#define MAX_DATA_LENGTH 1024

/**
 * @brief Callback function to handle the response data from the server.
 *
 * This function is used by libcurl to store the server response.
 *
 * @param ptr Pointer to the data received from the server.
 * @param size Size of each data element.
 * @param nmemb Number of elements.
 * @param userdata Pointer to the response buffer.
 * @return Size of the data received.
 */
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *userdata)
{
    size_t total_size = size * nmemb;
    strncat(userdata, ptr, total_size);
    return total_size;
}

/**
 * @brief Encodes data for application/x-www-form-urlencoded content type.
 *
 * This function URL encodes the data to ensure special characters are correctly transmitted.
 *
 * @param input The data to be encoded.
 * @return The URL encoded string.
 */
char *url_encode(const char *input)
{
    CURL *curl = curl_easy_init();
    char *encoded = NULL;
    if (curl)
    {
        encoded = curl_easy_escape(curl, input, 0);
    }
    return encoded;
}

/**
 * @brief Encodes data for application/json content type.
 *
 * This function encodes the data into JSON format.
 *
 * @param data The data to be encoded.
 * @return The JSON string.
 */
char *encode_json(const char *data)
{
    // Simple JSON encoding for demonstration (can be expanded).
    size_t len = strlen(data) + 3;  // {"data": "..."}
    char  *json = malloc(len);
    if (json)
    {
        snprintf(json, len, "{\"data\":\"%s\"}", data);
    }
    return json;
}

/**
 * @brief Decodes a JSON response.
 *
 * This function decodes a JSON response from the server.
 *
 * @param response The JSON response to be decoded.
 * @return The decoded data string.
 */
char *decode_json(const char *response)
{
    // Simple JSON decoding (can be expanded based on actual response format).
    const char *start = strchr(response, '"') + 1;  // Skip opening quote.
    const char *end = strchr(start, '"');
    size_t      len = end - start;
    char       *decoded = malloc(len + 1);
    if (decoded)
    {
        strncpy(decoded, start, len);
        decoded[len] = '\0';
    }
    return decoded;
}

/**
 * @brief Sends an HTTP GET request.
 *
 * @param url The URL to send the GET request to.
 * @param response The response from the server.
 * @return CURLcode The result of the GET request.
 */
CURLcode send_get_request(const char *url, const char *username, const char *password, int auth_type, char *response)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return CURLE_FAILED_INIT;
    }

    if (username && password)
    {
        if (auth_type == 0)
        {  // Basic Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
        else if (auth_type == 1)
        {  // Digest Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res;
}

/**
 * @brief Sends an HTTP POST request.
 *
 * @param url The URL to send the POST request to.
 * @param content_type The content type for the request.
 * @param data The data to be sent in the POST request.
 * @param username The username for authentication.
 * @param password The password for authentication.
 * @param auth_type The authentication type (Basic or Digest).
 * @param response The response from the server.
 * @return CURLcode The result of the POST request.
 */
CURLcode send_post_request(const char *url, const char *content_type, const char *data, const char *username, const char *password, int auth_type,
                           char *response)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return CURLE_FAILED_INIT;
    }

    struct curl_slist *headers = NULL;
    if (strcmp(content_type, "application/json") == 0)
    {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char *json_data = encode_json(data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        free(json_data);
    }
    else if (strcmp(content_type, "application/x-www-form-urlencoded") == 0)
    {
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        char *encoded_data = url_encode(data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, encoded_data);
        free(encoded_data);
    }

    if (username && password)
    {
        if (auth_type == 0)
        {  // Basic Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
        else if (auth_type == 1)
        {  // Digest Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res;
}

/**
 * @brief Sends an HTTP PUT request.
 *
 * @param url The URL to send the PUT request to.
 * @param content_type The content type for the request.
 * @param data The data to be sent in the PUT request.
 * @param username The username for authentication.
 * @param password The password for authentication.
 * @param auth_type The authentication type (Basic or Digest).
 * @param response The response from the server.
 * @return CURLcode The result of the PUT request.
 */
CURLcode send_put_request(const char *url, const char *content_type, const char *data, const char *username, const char *password, int auth_type,
                          char *response)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return CURLE_FAILED_INIT;
    }

    struct curl_slist *headers = NULL;
    if (strcmp(content_type, "application/json") == 0)
    {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char *json_data = encode_json(data);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        free(json_data);
    }
    else if (strcmp(content_type, "application/x-www-form-urlencoded") == 0)
    {
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        char *encoded_data = url_encode(data);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, encoded_data);
        free(encoded_data);
    }

    if (username && password)
    {
        if (auth_type == 0)
        {  // Basic Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
        else if (auth_type == 1)
        {  // Digest Authentication
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
            curl_easy_setopt(curl, CURLOPT_USERPWD, username);
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res;
}

/**
 * @brief Main function to parse command-line arguments and perform the HTTP request.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int 0 on success, 1 on failure.
 */
int main(int argc, char *argv[])
{
    if (argc < 7)
    {
        fprintf(stderr, "Usage: %s <method> <content_type> <url> <data> <ip> <port> <auth_type>\n", argv[0]);
        return 1;
    }

    const char *method = argv[1];
    const char *content_type = argv[2];
    const char *url = argv[3];
    const char *data = argv[4];
    const char *ip = argv[5];
    const char *port = argv[6];
    const char *auth_type_str = argv[7];

    char full_url[MAX_URL_LENGTH];
    snprintf(full_url, sizeof(full_url), "http://%s:%s%s", ip, port, url);

    // Get username and password from user
    char username[100], password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    int auth_type = 0;  // Default to Basic Authentication
    if (strcmp(auth_type_str, "digest") == 0)
    {
        auth_type = 1;  // Digest Authentication
    }

    char response[MAX_DATA_LENGTH] = {0};

    CURLcode res;
    if (strcmp(method, "GET") == 0)
    {
        res = send_get_request(full_url, username, password, auth_type, response);
    }
    else if (strcmp(method, "POST") == 0)
    {
        res = send_post_request(full_url, content_type, data, username, password, auth_type, response);
    }
    else if (strcmp(method, "PUT") == 0)
    {
        res = send_put_request(full_url, content_type, data, username, password, auth_type, response);
    }
    else
    {
        fprintf(stderr, "Unsupported method: %s\n", method);
        return 1;
    }

    if (res != CURLE_OK)
    {
        fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        return 1;
    }

    printf("Response: %s\n", response);

    return 0;
}
