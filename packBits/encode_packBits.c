#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RUN_LENGTH 128  // Maximum run length in PackBits (127 for positive)

int packbits_encode(const uint8_t *input, size_t input_len, uint8_t *output)
{
    size_t i = 0, out_pos = 0;

    while (i < input_len)
    {
        // Start a run of repeated bytes if possible
        size_t run_length = 1;
        if (i + 1 < input_len && input[i] == input[i + 1])
        {
            // Find the length of the run
            while (i + run_length < input_len && run_length < MAX_RUN_LENGTH && input[i] == input[i + run_length])
            {
                run_length++;
            }
            // Encode run as a negative length and the repeated byte
            output[out_pos++] = (uint8_t)(-(int32_t)run_length + 1);
            output[out_pos++] = input[i];
            i += run_length;
        }
        else
        {
            // Encode non-repeating bytes directly
            size_t non_run_length = 0;
            while (i + non_run_length < input_len && non_run_length < MAX_RUN_LENGTH &&
                   (i + non_run_length + 1 >= input_len || input[i + non_run_length] != input[i + non_run_length + 1]))
            {
                non_run_length++;
            }
            output[out_pos++] = (uint8_t)(non_run_length - 1);
            memcpy(&output[out_pos], &input[i], non_run_length);
            out_pos += non_run_length;
            i += non_run_length;
        }
    }
    return out_pos;
}

int main()
{
    uint8_t input[] = {0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0};
    size_t  input_len = sizeof(input);
    uint8_t output[256];  // Buffer to hold the compressed data

    int output_len = packbits_encode(input, input_len, output);

    printf("Encoded data: ");
    for (int i = 0; i < output_len; i++)
    {
        printf("%02X ", output[i]);
    }
    printf("\n");

    return 0;
}
