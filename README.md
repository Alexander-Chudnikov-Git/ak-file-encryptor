# AK File Encryptor

**AK File Encryptor** is a terminal-based file encryption and decryption tool built using the [libakrypt](https://libakrypt.org/) cryptographic library. It features a user-friendly interface implemented with `ncurses` to handle files, strings, and directories securely.

---

## Features

- **Encrypt or Decrypt Files**: Securely encrypt or decrypt files with custom keys.
- **String Processing**: Encrypt or decrypt individual strings.
- **Directory Support**: Encrypt or decrypt all files in a root directory.
- **Interactive Menu**: Intuitive navigation via an `ncurses`-based menu system.
- **Key Management**: Generate keys from passwords or random values.

---

## Installation

### Prerequisites

- **C++ Compiler**: Ensure GCC or Clang is installed.
- **CMake**: Version 3.15 or later.
- **libakrypt**: Install via [package manager](https://aur.archlinux.org/packages/libakrypt) or [build from source](https://libakrypt.ru/install-guide.html).
- **ncurses**: Install via package manager.

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/<your-username>/ak-file-encryptor.git
   cd ak-file-encryptor
   ```

2. Create a build directory and compile the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. Run the executable:
   ```bash
   ./ak-file-encryptor
   ```

---

## Usage

### Main Menu
```
Select what do you want to process?
-----------------------------------
  1 - File
  2 - String
  3 - Root folder
  q - Exit
  Time left: 10 seconds
```

### Encryption/Decryption
After selecting a target (File, String, or Folder):
```
Do you want to encrypt or decrypt?
-----------------------------------
  e - Encrypt
  d - Decrypt
  r - Return
  q - Exit
  Time left: 10 seconds
```

### File Selection
For files or folders, provide a valid path:
```
Please select file
-----------------------------------
Enter file path: /path/to/your/file
```

### Password-Based Key Generation
After selecting an operation (encrypt/decrypt) and providing a file, you will be prompted to enter an encryption password or generate random one:
```
Enter encryption password
-----------------------------------
  File: text.txt
  Password: OGlVVQrYPWiSGElG5edNJ1jzvnfy7bpD
  Key: 2b fa aa 85 a5 69 af b5 e0 ac f4...
  Result: 'test ' -> '~C- a'
```


---

## Project Structure

- **`main_menu.hpp`**: Handles the interactive `ncurses` menu.
- **`crypto_provider.hpp`**: Wraps cryptographic functions for libakrypt, simplifying encryption and decryption operations.
- **`src/`**: Source code for both UI and backend logic.
- **`docs/`**: Documentation files for the project.

---

## Known Issues

- The project contains some legacy code and potential bugs.
- Limited error handling in cases of invalid input or corrupted files.
