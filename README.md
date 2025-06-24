# 🔐 encrypted_block<T>

A lightweight C++ wrapper for **runtime obfuscation of sensitive data**, designed to protect values like integers, pointers, syscall indices, and more from memory scanning, static analysis, and reverse engineering.

---

## 💡 Why Use It?

In low-level and anti-cheat development, even something as simple as:

```cpp
int value = 1337;
```

...can be found in memory or disassembly. Tools like IDA will show this clearly, making it easy for attackers to:

- Scan for known constants  
- Patch logic based on value checks  
- Create memory signatures to bypass protections  

---

## 🛡️ What `encrypted_block<T>` Does

This class stores any data of type `T` in an **encrypted format**, using:

- Randomized per-instance keys  
- Per-byte XOR + rotation  
- On-demand decryption only when needed  

The result:
- No plaintext values in `.rdata`, `.data`, or globals  
- No easy-to-find constants in memory  
- No static signature to scan for  

---

## 🧠 Stack-Based by Design

When used inside functions or as arguments:
- All memory stays on the **stack**  
- No heap or global allocations  
- No constant values exist unless you decrypt manually  

Even decrypted values only briefly exist in **registers or the stack**.

---

## ✨ Features

- 🧬 Per-instance encryption  
- 🔁 XOR + bit-rotation per byte  
- 💥 No dependencies (CRT-free compatible)  
- 🔒 Protects values from static inspection  
- 🧩 Works with any trivially copyable type  
- ✅ Supports assignment and comparisons  

---

## 🔧 Usage

```cpp
#include "encrypted_block.h"

int main() {
    encrypted_block<int> block(1337);

    if (block == 1337) {
        std::cout << "Match: " << block.get_decrypted() << std::endl;
    }

    block = 9001;
    std::cout << "Updated value: " << block.get_decrypted() << std::endl;

    return 0;
}
```

---

## 🔬 Disassembly Comparison (IDA)

### ❌ Without Obfuscation:
```asm
mov eax, 1337
```

### ✅ With `encrypted_block<int>`:
```asm
rdrand, xor, rotate, qmemcpy...
; 1337 is hidden in these instructions making this extremely difficult to reverse 
```

Everything is dynamically built at runtime using entropy and randomized transformations.

---

## 🚀 Ideal For

- Anti-cheat systems  
- DRM and security wrappers  
- VM/emulator resistance  
- Static signature evasion  
- Custom obfuscators and loaders  

---

## 📄 License

This project is provided for educational and security research purposes. Please use responsibly.

---

## 🤝 Let's Connect

If you're working on:
- Game security  
- Reverse engineering protection  
- Custom obfuscation  
- Anti-debugging techniques  

Feel free to reach out or contribute!

![Screenshot 2025-06-24 193408](https://github.com/user-attachments/assets/c07af1c0-a405-4279-868e-5a2a912b7b06)
