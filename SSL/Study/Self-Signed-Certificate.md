# Self-Signed Certificate

## What is Self-Signed Certificate?

- A self-signed SSL / TLS certificate is a digital certificate that’s not signed by a publicly trusted Certificate Authority (CA).  
- They are created, issued, and signed by the company or developer who is responsible for the website or software associated with the certificate, rather than a trusted CA.
- At a high-level, these self-signed certificates are based on the same cryptographic private and public key pair architecture that’s used in X.509 certificates.  
- They are considered unsafe for public-facing websites and applications and are typically used only in internal development and testing environments.

## About X.509 Certificate  

- X.509 certificates are a standard format for digital certificates that are widely used for securing communications and establishing trust over networks, especially in protocols like HTTPS, email encryption, and VPNs. These certificates contain information that allows a party to verify the identity of the certificate holder.  
  
    __"X":__ Refers to the X-series of recommendations, which covers various standards related to data networks, especially in the context of telecommunications and public key infrastructure (PKI).
    __"509":__ This is the specific recommendation number in the X-series that pertains to the standard for public key certificates. It was first introduced in 1988 by the ITU-T as part of the X.500 directory services standard. Over time, it evolved into the widely used standard for digital certificates.  

### An X.509 certificate typically includes the following  

- __Subject:__ The entity (person, organization, or device) that the certificate represents, often containing information like the Common Name (CN), organization (O), and location.
- __Issuer:__ The Certificate Authority (CA) that issued and signed the certificate, validating its authenticity.
- __Public Key:__ The public key that can be used to establish secure communication, often associated with the private key held by the subject.
- __Serial Number:__ A unique identifier for the certificate.
- __Validity Period:__ The dates during which the certificate is valid (the start and expiration dates).
- __Signature:__ A digital signature from the issuer (CA) that confirms the authenticity and integrity of the certificate.
- __Extensions:__ Additional information, such as usage constraints, key usage, or alternate names.

These certificates are commonly used in __Public Key Infrastructure (PKI)__ systems to establish secure encrypted connections and authenticate identities. PKI is a framework of technologies, policies, and procedures designed to manage digital certificates and public-key encryption for secure communication, authentication, and data integrity.

## Essential Fields for Self-Signed Certificates in Local Security Applications  

__1. Subject (Distinguished Name)__  
__Fields:__ CN (Common Name), O (Organization), L (Locality), ST (State or Province), C (Country).
__Reason:__ These fields identify the entity for which the certificate is issued. While self-signed certificates don't require the validation from a Certificate Authority (CA), they still need to contain some form of identity. For local applications, the CN might be your domain name, hostname, or application identifier. The O field should reflect the organization or team responsible for the application.

__2. Public Key__
__Fields:__ Public Key
__Reason:__ This key is used for encrypting and verifying signatures. The public key must be part of the self-signed certificate, and it will be used by other entities (such as your application or other systems) to encrypt data or verify that a digital signature matches the expected value.

__3. Serial Number__
__Fields:__ Serial Number
__Reason:__ A unique identifier for the certificate. While this field is mostly used for distinguishing certificates in a CA-issued context, it's still necessary for uniqueness and traceability. Even in a local setup, it helps differentiate the certificates.

__4. Validity Period__
__Fields:__ Not Before, Not After
__Reason:__ These fields define the time-frame during which the certificate is valid. Setting an appropriate expiration date (e.g., 1 year or more) is important to ensure periodic renewal and secure lifecycle management.

__5. Signature Algorithm__
__Fields:__ Signature Algorithm (e.g., SHA256-RSA or SHA512-RSA)
__Reason:__ This defines the algorithm used to sign the certificate. While a self-signed certificate doesn’t require external verification, it’s still important to choose a strong, secure signature algorithm like SHA-256 or SHA-512 to ensure the certificate's integrity and authenticity.

__6. Extensions (Optional, but Recommended)__  

- __Key Usage:__ This specifies the intended use of the public key, such as digitalSignature, keyEncipherment, or dataEncipherment.
- __Subject Alternative Name (SAN):__ If applicable, this allows you to include alternate names (e.g., IP addresses or other domain names) that the certificate will be valid for. This is useful if the application or system may be accessed via multiple names.
- __Basic Constraints:__ Useful for defining whether the certificate is a CA certificate or not. For self-signed certificates that are not used to sign other certificates, the CA:FALSE setting is appropriate.
- __Extended Key Usage (EKU):__ The EKU extension specifies the allowed usages for the public key, adding an extra layer of security and ensuring that the certificate is used only for its intended purposes. Without EKU, a certificate could be misused in unintended scenarios. Defining the EKU restricts the certificate's usage to a specific context.

Adding such extensions help clarify the intended purpose and scope of the certificate. For a security product, defining these correctly can prevent improper usage and help ensure the certificate is only used for its intended purposes.

## OpenSSL commands to generate self-signed certificate

### Generating a private key

```bash
openssl genpkey -algorithm RSA -out private.key -pkeyopt rsa_keygen_bits:2048
```

### Generating self-signed certificate

```bash
openssl req -new -x509 -key private.key -out certificate.crt -days 365
```

> You will be prompted to provide the following information for the certificate:
>
> - Country Name (2-letter code) [e.g., US]
> - State or Province Name [e.g., California]
> - Locality Name (e.g., city) [e.g., San Francisco]
> - Organization Name (e.g., company) [e.g., MyCompany]
> - Organizational Unit Name (e.g., section) [e.g., IT Department]
> - Common Name (e.g., fully qualified domain name) [e.g., www.example.com]
> - Email Address [e.g., admin@example.com]

### Converting Certificate/Key Format

> Converting PEM to DER

```bash
openssl x509 -in cert.pem -outform DER -out cert.der
openssl rsa -in private_key.pem -outform DER -out private_key.der
openssl pkcs12 -in keystore.p12 -clcerts -nokeys -out cert.der
```

> Converting DER to PEM

```bash
openssl x509 -in cert.der -inform DER -outform PEM -out cert.pem
openssl rsa -in private_key.der -inform DER -outform PEM -out private_key.pem
openssl pkcs12 -in keystore.p12 -clcerts -nokeys -out cert.pem
```
