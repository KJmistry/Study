#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_mac_address(char *buffer, int len)
{
    // Assuming the MAC address starts at a known position in the buffer
    // (this is a simplified assumption for illustrative purposes)
    unsigned char mac_address[6];

    // Example: let's assume MAC address is located starting at byte 14 (adjust as necessary)
    int mac_offset = 14;

    if (len > mac_offset + 6)
    {
        // Copy 6 bytes from the raw data as the MAC address
        memcpy(mac_address, buffer + mac_offset, 6);

        // Print MAC address
        printf("MAC Address: ");
        for (int i = 0; i < 6; i++)
        {
            printf("%02x", mac_address[i]);
            if (i < 5)
            {
                printf(":");
            }
        }
        printf("\n");
    }
    else
    {
        printf("Error: Buffer too small to extract MAC address.\n");
    }
}

int main()
{
    // Raw data from the kernel (example received data)
    char buffer[] = {0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x59, 0xff, 0xff, 0xcf, 0x06,
                     0x00, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff, 0x1c, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x01, 0x00,
                     0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc5, 0x70, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe,
                     0x00, 0x00, 0x00, 0xff, 0xff, 0xd0, 0xff, 0xff, 0xf4, 0xff, 0xff, 0xdb, 0xff, 0xff, 0xde};

    int len = sizeof(buffer);

    // Extract the MAC address from the raw data
    extract_mac_address(buffer, len);

    return 0;
}