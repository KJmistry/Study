# 🔐 Cryptographic Essentials for SSL & Security Applications

## Introduction  
This document provides an essential **overview of cryptographic concepts** used in **SSL/TLS** and other **secure applications**.  
Understanding these concepts is **crucial** before implementing or working with cryptographic applications.

---

## 1. Hashing Algorithms

### 🔹 **What is Hashing?**  
Hashing is a one-way function that converts input data into a **fixed-length hash** (digest). It is commonly used for **data integrity** and **password hashing**.

### 🔹 **Common Hashing Algorithms:**
| Algorithm   | Output Size | Use Case |
|------------|------------|----------|
| **SHA-1**  | 160-bit    | ⚠️ Weak, outdated |
| **SHA-256** | 256-bit  | ✅ Secure, widely used in SSL, blockchain |
| **SHA-512** | 512-bit  | ✅ Stronger than SHA-256 |
| **MD5**    | 128-bit    | ⚠️ Weak, only used for checksums |
| **BLAKE2** | 256-bit    | ✅ Faster, alternative to SHA-3 |

**Example Usage:**
```bash
openssl dgst -sha256 file.txt
```

---

## 2. Encryption Algorithms

### 🔹 **Types of Encryption:**
- **Symmetric Encryption** (Same key for encryption & decryption)
- **Asymmetric Encryption** (Public-Private key pair)

### 🔹 **Symmetric Encryption Algorithms:**
| Algorithm  | Key Size | Use Case |
|------------|---------|----------|
| **AES**    | 128/256-bit | ✅ Used in SSL, VPNs, disk encryption |
| **DES**    | 56-bit  | ⚠️ Weak, replaced by AES |
| **3DES**   | 168-bit | ⚠️ Slower than AES, legacy systems |
| **ChaCha20** | 256-bit | ✅ Used in TLS, faster than AES in software |

**Example Usage:**
```bash
openssl enc -aes-256-cbc -salt -in secret.txt -out encrypted.txt
```

### 🔹 **Asymmetric Encryption Algorithms:**
| Algorithm  | Key Size | Use Case |
|------------|---------|----------|
| **RSA**    | 2048/4096-bit | ✅ Used in SSL, digital signatures |
| **ECC (Elliptic Curve Cryptography)** | 256-bit | ✅ Faster & secure alternative to RSA |
| **DSA**    | 1024-3072-bit | ✅ Digital signatures, SSH |

**Example RSA Key Generation:**
```bash
openssl genpkey -algorithm RSA -out private.key
```

---

## 3. Digital Signatures ✍️  
Digital signatures **prove authenticity and integrity** of messages or files.

### 🔹 **How It Works?**
1. Sender **hashes** the data.
2. Sender **encrypts** the hash using a **private key** (signature).
3. Receiver **decrypts** using sender’s **public key**.
4. Receiver **compares the hash** to verify integrity.

**Example RSA Signature:**
```bash
openssl dgst -sha256 -sign private.key -out signature.bin file.txt
```

---

## 4. Key Exchange Algorithms
Key exchange is used to securely share encryption keys between two parties.

### 🔹 **Common Key Exchange Methods:**
| Algorithm | Use Case |
|-----------|---------|
| **DH (Diffie-Hellman)** | Secure key exchange in TLS |
| **ECDH (Elliptic Curve Diffie-Hellman)** | ⚡ Faster & more secure than DH |

**Example DH Key Exchange:**
```bash
openssl dhparam -out dhparam.pem 2048
```

---

## 5. SSL/TLS Certificates  
SSL/TLS certificates use **asymmetric encryption** to secure web traffic.

### 🔹 **Types of Certificates:**
| Type | Use Case |
|------|---------|
| **Self-Signed** | For testing, internal use |
| **CA-Signed** | Trusted by browsers, requires a Certificate Authority (CA) |
| **Wildcard** | Secures multiple subdomains (`*.example.com`) |

**Generate a Self-Signed Certificate:**
```bash
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365
```

---

## 6. Public Key Infrastructure (PKI) 
PKI is the system that **manages digital certificates & encryption keys**.

### 🔹 **Key Components:**
- **CA (Certificate Authority)** – Issues digital certificates.
- **CSR (Certificate Signing Request)** – Request for a CA-signed certificate.
- **CRL (Certificate Revocation List)** – Lists revoked certificates.

**Example CSR Generation:**
```bash
openssl req -new -key private.key -out request.csr
```

---

## 7. OpenSSL Verification Commands  

### 🔹 **Verify a Self-Signed Certificate**
```bash
openssl verify -CAfile cert.pem cert.pem
```

### 🔹 **Check Certificate Details**
```bash
openssl x509 -in cert.pem -text -noout
```

### 🔹 **Check if a Private Key Matches a Certificate**
```bash
openssl rsa -noout -modulus -in private.key | openssl md5
openssl x509 -noout -modulus -in cert.pem | openssl md5
```

---

## 8. Secure Password Hashing 

### 🔹 **Recommended Methods:**
| Algorithm | Use Case |
|-----------|---------|
| **bcrypt** | Secure password storage |
| **PBKDF2** | Used in Wi-Fi WPA2 encryption |
| **Argon2** | ✅ Strongest, modern alternative |

**Example Hashing a Password with OpenSSL:**
```bash
openssl passwd -6 mypassword
```

---

## 9. Common Attacks & Prevention 
| Attack | Description | Prevention |
|--------|------------|------------|
| **MITM (Man-in-the-Middle)** | Attacker intercepts communication | ✅ Use HTTPS, strong encryption |
| **Brute Force** | Guessing passwords | ✅ Use strong passwords, bcrypt |
| **Hash Collision** | Different inputs giving the same hash | ✅ Use **SHA-256** instead of **MD5/SHA-1** |
| **Weak RSA Keys** | Small key size is breakable | ✅ Use **2048-bit or higher** |

---
