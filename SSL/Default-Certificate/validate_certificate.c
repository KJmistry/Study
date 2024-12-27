
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <stdio.h>
#include <unistd.h>

#define RSA_KEY_SIZE               2048
#define SERIAL_NUMBER_SIZE         64  // Bytes
#define CERT_EXPIRE_DURATION_YEARS 10

#define FAIL                       1
#define SUCCESS                    0

int compare_asn1_time_to_time_t(ASN1_TIME *asn1_time, time_t current_time)
{
    struct tm tm_time;
    time_t    asn1_time_t;
    int       year, month, day, hour, minute, second;
    char     *str = (char *)ASN1_STRING_data(asn1_time);
    int       length = ASN1_STRING_length(asn1_time);

    memset(&tm_time, 0, sizeof(struct tm));

    // ASN1_TIME format is YYMMDDHHMMSSZ
    // The last character is always 'Z' (indicating UTC)
    if (length == 13)
    {
        // Parse the time
        sscanf(str, "%2d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);
        year += 2000;  // Adjust for 2-digit year
    }
    else if (length == 15)
    {
        // Parse the time (includes a 4-digit year)
        sscanf(str, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);
    }
    else
    {
        return -2;  // Invalid ASN1_TIME format
    }

    // Populate the tm struct
    tm_time.tm_year = year - 1900;  // tm_year is years since 1900
    tm_time.tm_mon = month - 1;     // tm_mon is months since January (0-11)
    tm_time.tm_mday = day;          // tm_mday is the day of the month (1-31)
    tm_time.tm_hour = hour;         // tm_hour is the hour of the day (0-23)
    tm_time.tm_min = minute;        // tm_min is the minute (0-59)
    tm_time.tm_sec = second;        // tm_sec is the second (0-59)
    tm_time.tm_isdst = -1;          // tm_isdst is for daylight saving time, set to -1 to auto detect

    // Convert struct tm to time_t
    asn1_time_t = mktime(&tm_time);

    if (asn1_time_t == -1)
    {
        printf("mktime failed: [err=%s]", strerror(errno));
        return -2;
    }

    // Compare with current time
    if (asn1_time_t > current_time)
    {
        return 1;  // ASN1_TIME is later than current_time
    }
    else if (asn1_time_t < current_time)
    {
        return -1;  // ASN1_TIME is earlier than current_time
    }
    else
    {
        return 0;  // Times are equal
    }
}

/**
 * @brief Validate an existing certificate file for proper formatting and expiration.
 * @param certPath The path to the certificate file to validate.
 * @return BOOL SUCCESS if the certificate is valid, FAIL otherwise.
 */
int validateCertificate(const char *certPath)
{
    FILE      *pCertFile = NULL;
    X509      *x509 = NULL;
    int        status = FAIL;
    ASN1_TIME *notBefore, *notAfter;
    time_t     currentTime;
    int        ret;

    do
    {
        /* Check if certificate file exists */
        if (access(certPath, F_OK) != 0)
        {
            printf("certificate file does not exist: [path=%s]", certPath);
            break;
        }

        /* Open the certificate file */
        pCertFile = fopen(certPath, "rb");
        if (NULL == pCertFile)
        {
            printf("unable to open certificate file: [path=%s]", certPath);
            break;
        }

        /* Read the certificate from the file */
        x509 = PEM_read_X509(pCertFile, NULL, NULL, NULL);
        if (NULL == x509)
        {
            printf("failed to parse X.509 certificate from file: [path=%s]", certPath);
            break;
        }

        /* Get the validity period (notBefore and notAfter) */
        notBefore = X509_get_notBefore(x509);
        notAfter = X509_get_notAfter(x509);

        if ((NULL == notBefore) || (NULL == notAfter))
        {
            printf("certificate validity period not found");
            break;
        }

        if (ASN1_TIME_check(notBefore) != 1)
        {
            printf("invalid noBefore");
            break;
        }

        /* Check if the certificate's end time (notAfter) is in the past */
        if (ASN1_TIME_check(notAfter) != 1)
        {
            printf("invalid noAfter");
            break;
        }

        /* Get the current time */
        currentTime = time(NULL);

        // /* Check if the certificate is valid now (currentTime >= notBefore) */
        // ret = ASN1_TIME_cmp_time_t(notBefore, currentTime);
        // if (ret > 0)
        // {
        //     printf("certificate is not valid yet");
        //     break;
        // }

        // /* Check if the certificate is expired (currentTime <= notAfter) */
        // ret = ASN1_TIME_cmp_time_t(notAfter, currentTime);
        // if (ret < 0)
        // {
        //     printf("certificate has expired");
        //     break;
        // }

        ret = compare_asn1_time_to_time_t(notBefore, currentTime);
        if ((ret == -2) || (ret == 1))
        {
            printf("certificate is not valid yet\n");
            break;
        }

        ret = compare_asn1_time_to_time_t(notAfter, currentTime);
        if ((ret == -2) || (ret == -1))
        {
            printf("certificate has expired\n");
            break;
        }

        /* Certificate is valid */
        printf("certificate is valid: [path=%s]", certPath);
        status = SUCCESS;

    } while (0);

    /* Cleanup */
    if (pCertFile)
    {
        fclose(pCertFile);
    }

    if (x509)
    {
        X509_free(x509);
    }

    return status;
}

int main()
{
    const char *cert_file = "2040-2050server.crt";
    // const char *cert_file = "2001-2011server.crt";
    // const char *cert_file = "server.crt";

    /* Generate RSA private key and X.509 certificate */

    if (SUCCESS == validateCertificate(cert_file))
    {
        printf("Certificate is valid.\n");
    }
    else
    {
        printf("Certificate is Invalid.\n");
    }

    return 0;
}
