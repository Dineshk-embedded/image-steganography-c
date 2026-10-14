# Image Steganography in C

A console-based C application that hides a secret file inside a 24-bit BMP image using the LSB (Least Significant Bit) technique, and extracts it back. The image looks the same to the human eye.

## Features
- **Encode:** hide a secret file inside a BMP image
- **Decode:** recover the hidden file from the stego image
- Capacity check, so the secret file must fit inside the image
- Magic string (`#*`) to detect whether an image contains hidden data
- Stores the secret file's extension and size inside the image
- Validates file extensions and command-line arguments
- Optional output file names for both encoding and decoding

## How It Works
1. The 54-byte BMP header is copied unchanged.
2. Each bit of the data replaces the least significant bit of one image byte.
3. Data is stored in this order: magic string, extension size, extension, file size, file data.
4. The rest of the image is copied as is.

Decoding reads the same bits in the same order and rebuilds the file.

## Functions Overview
| Function | Purpose |
|---|---|
| `check_operation_type()` | Detects `-e` (encode) or `-d` (decode) |
| `read_and_validate_encode_args()` | Validates source, secret and stego file names |
| `check_capacity()` | Checks that the image can hold the secret data |
| `copy_bmp_header()` | Copies the 54-byte BMP header |
| `encode_magic_string()` | Hides the magic string in the image |
| `encode_secret_file_extn_size()` / `encode_secret_file_extn()` | Hide the extension size and extension |
| `encode_secret_file_size()` / `encode_secret_file_data()` | Hide the file size and file data |
| `encode_byte_to_lsb()` / `encode_size_to_lsb()` | Write a byte or a size into image LSBs |
| `copy_remaining_img_data()` | Copies the rest of the image |
| `read_and_validate_decode_args()` | Validates decode arguments |
| `decode_magic_string()` | Confirms the image has hidden data |
| `decode_secret_file_extn_size()` / `decode_secret_file_extn()` | Recover the extension info |
| `decode_secret_file_size()` / `decode_secret_file_data()` | Recover the file size and data |
| `decode_byte_from_lsb()` / `decode_size_from_lsb()` | Read a byte or a size from image LSBs |

## Tech Used
- Language: C
- Concepts: bitwise operators, binary file I/O, structures, enums, command-line arguments, BMP file format
- Compiler: gcc

## How to Build and Run
```bash
gcc *.c -o stego

# Encode
./stego -e beautiful.bmp secret.txt stego.bmp

# Decode
./stego -d stego.bmp output.txt
```
The last file name is optional in both commands.

## Sample Output
```
=============================
           ENCODING
=============================
Source Image     : beautiful.bmp
Secret File      : secret.txt
Stego Image      : stego.bmp

width = 1024    height = 768
Copy Bmp Header                  : Success
Encoding Magic String            : Success
Encoding secret file extn size   : Success
Encoding secret file extn        : Success
Encoding secret file Size        : Success
Encoding secret file Data        : Success
Copy Remaining img data          : Success
```

## Limitations
- Works only with 24-bit BMP images
- Secret file extension can be at most 3 letters (e.g. `.txt`, `.c`, `.pdf`)

## Future Improvements
- Support PNG images
- Encrypt the secret data before hiding it
- Add password protection
- Allow longer file extensions
- Support hiding multiple files
