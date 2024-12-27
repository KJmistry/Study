#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Function to decode PackBits
void decodePackBits(uint8_t *encoded, size_t encoded_len, uint8_t **decoded, size_t *decoded_len)
{
    size_t decoded_size = 0;
    size_t decoded_capacity = 1024;  // Initial capacity for the decoded buffer
    *decoded = (uint8_t *)malloc(decoded_capacity);

    if (*decoded == NULL)
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    size_t i = 0;
    while (i < encoded_len)
    {
        uint8_t byte = encoded[i++];

        if (byte >= 0x80)
        {                                // Repeat next byte (0x80 to 0xFF)
            int     count = 257 - byte;  // Number of times to repeat (257 - byte value)
            uint8_t repeat_byte = encoded[i++];
            for (int j = 0; j < count; j++)
            {
                (*decoded)[decoded_size++] = repeat_byte;
            }
        }
        else
        {                          // Literal bytes (0x00 to 0x7F)
            int count = byte + 1;  // Number of literal bytes
            for (int j = 0; j < count; j++)
            {
                (*decoded)[decoded_size++] = encoded[i++];
            }
        }

        // Ensure the decoded array has enough capacity
        if (decoded_size >= decoded_capacity)
        {
            decoded_capacity *= 2;  // Double the capacity
            *decoded = (uint8_t *)realloc(*decoded, decoded_capacity);
            if (*decoded == NULL)
            {
                perror("Failed to realloc memory");
                exit(1);
            }
        }
    }

    *decoded_len = decoded_size;
}

int main()
{
    uint8_t encoded[] = {0xFE, 0xFF, 0xFE, 0xF0};
    // uint8_t  encoded[] = {0xD0, 0xFF, 0x00, 0xF0};
    size_t   encoded_len = sizeof(encoded) / sizeof(encoded[0]);
    uint8_t *decoded = NULL;
    size_t   decoded_len = 0;

    // Decode PackBits
    decodePackBits(encoded, encoded_len, &decoded, &decoded_len);

    printf("Decoded len: %ld\n", decoded_len);

    // Print decoded output
    printf("Decoded: ");
    for (size_t i = 0; i < decoded_len; i++)
    {
        printf("%02X ", decoded[i]);
    }
    printf("\n");

    // Free the allocated memory
    free(decoded);

    return 0;
}
