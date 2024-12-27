#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to handle the HTTP response
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data)
{
    // Write the response data into the provided buffer (data)
    strcat(data, (char *)ptr);
    return size * nmemb;
}

// Function to parse the response and extract tic-value and tac-value
int parse_response(const char *response, char *tic_value, char *tac_value)
{
    // Parse the response string to extract the tic-value and tac-value
    const char *tic_key = "tic-value=";
    const char *tac_key = "tac-value=";

    char *tic_start = strstr(response, tic_key);
    char *tac_start = strstr(response, tac_key);

    if (tic_start && tac_start)
    {
        tic_start += strlen(tic_key);  // Move pointer to start of actual value
        tac_start += strlen(tac_key);  // Move pointer to start of actual value

        // Find the end of tic-value and tac-value (the next newline or end of string)
        char *tic_end = strstr(tic_start, "\n");
        char *tac_end = strstr(tac_start, "\n");

        if (tic_end)
        {
            size_t tic_len = tic_end - tic_start;
            strncpy(tic_value, tic_start, tic_len);
            tic_value[tic_len] = '\0';
        }
        else
        {
            strcpy(tic_value, tic_start);
        }

        if (tac_end)
        {
            size_t tac_len = tac_end - tac_start;
            strncpy(tac_value, tac_start, tac_len);
            tac_value[tac_len] = '\0';
        }
        else
        {
            strcpy(tac_value, tac_start);
        }

        return 0;  // Success
    }

    return -1;  // Parsing error
}

int main()
{
    CURL    *curl;
    CURLcode res;
    char    *url = "http://192.168.101.55/login";  // Replace with your target IP
    char    *data = "action=gettictac&encpswdindx=1";
    char     response[1024] = {0};  // Buffer to store the response from the server
    char     tic_value[256] = {0};  // To store the extracted tic-value
    char     tac_value[256] = {0};  // To store the extracted tac-value

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Set timeout
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);

        // Set the Content-Type header (application/x-www-form-urlencoded)
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        // Perform the POST request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK)
        {
            // Parse the response and extract the tic and tac values
            if (parse_response(response, tic_value, tac_value) == 0)
            {
                printf("TIC Value: %s\n", tic_value);
                printf("TAC Value: %s\n", tac_value);
            }
            else
            {
                printf("Failed to parse response\n");
            }
        }
        else
        {
            // If the request failed
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    // Global cleanup
    curl_global_cleanup();

    return 0;
}
