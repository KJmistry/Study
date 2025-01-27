# README

## Overview
This guide provides step-by-step instructions to write, compile, and insert a minimal Linux kernel module while resolving the "Key was rejected by service" error caused by Secure Boot.

---

## Steps Followed to Write and Insert the Kernel Module

1. **Write a Minimal Kernel Module**
   - Created `myModule.c` to print kernel debug messages when the module is inserted and removed.

2. **Create a Makefile**
   - To compile the module using a Makefile.

3. **Compile the Kernel Module**
   - Ran the `make` command, which generated several files, including `myModule.ko` (located in the `myKernelModule` directory).

4. **Attempted to Insert the Module**
   - Command:
     ```sh
     sudo insmod myModule.ko
     ```
   - Output:
     ```sh
     insmod: ERROR: could not insert module myModule.ko: Key was rejected by service
     ```
   - **Error Cause:** The system is running Secure Boot, which enforces strict signature verification for kernel modules.

---

## Solution to Resolve the Error

There are two possible solutions:

1. **Disable Secure Boot** (Requires BIOS/UEFI access)
2. **Sign the Kernel Module with a New Key and Enroll It Using `mokutil`**

We will follow the second approach.

---

## Signing and Enrolling the Kernel Module

### **Step 1: Generate a Key Pair**
```sh
openssl req -new -x509 -newkey rsa:2048 -keyout MOK.priv -out MOK.pem -nodes -days 36500 -subj "/CN=MyKernelModule/"
```

### **Step 2: Sign the Kernel Module**
```sh
/usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 MOK.priv MOK.pem myModule.ko
```

### **Step 3: Convert the PEM Certificate to DER Format**
```sh
openssl x509 -outform der -in MOK.pem -out MOK.der
```

### **Step 4: Import the Public Key into the System's MOK List**
```sh
sudo mokutil --import MOK.der
```

### **Step 5: Reboot and Enroll the Key**
- After rebooting, a blue screen titled **"Perform MOK management"** will appear.
- Select **"Enroll MOK"**.
- Enter the password that was set during the certificate import.
- Continue to boot.

### **Step 6: Verify the Enrolled Key**
```sh
mokutil --list-enrolled
```
- You should see the certificate details in the output.

---

## Final Step: Insert the Module Again
```sh
sudo insmod myModule.ko
```

### **Verify the Module Load in Kernel Logs**
```sh
sudo dmesg | tail -10
```

If everything was done correctly, your module should now be successfully inserted!

---

## Conclusion
By following these steps, you have successfully written, compiled, signed, and inserted a Linux kernel module while ensuring compatibility with Secure Boot. This guide helps overcome the "Key was rejected by service" error using MOK (Machine Owner Key) enrollment.

Happy kernel hacking! 🚀
