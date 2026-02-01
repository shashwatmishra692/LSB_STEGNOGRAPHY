# LSB Steganography using C

## Project Overview
This project implements image-based steganography using the Least Significant Bit (LSB)
technique. It allows hiding secret data inside a BMP image and later extracting the
hidden data without any noticeable change in image quality.

The project is developed in C and focuses on low-level concepts such as file handling,
bitwise operations, and binary data manipulation.

---

## Features
- Encode secret text or file into a BMP image
- Decode and extract hidden data from the stego image
- Uses Least Significant Bit (LSB) technique
- No visible distortion in the output image
- Implemented using standard C file I/O
- Modular design with multiple source files

---

## Working Principle
In LSB steganography, the least significant bit of each byte of the image data is modified
to store one bit of the secret data. Since the LSB contributes the least to the overall
value of a byte, changing it does not cause any visible difference in the image.

During encoding, each bit of the secret data is embedded into the LSB of consecutive
image bytes. During decoding, these LSBs are extracted and combined to reconstruct
the original secret data.

---


## Project Structure

```

| File / Folder | Description |
|--------------|-------------|
| `main.c`     | Entry point of the application |
| `encode.c`   | Logic for encoding secret data |
| `decode.c`   | Logic for decoding hidden data |
| `stego.h`    | Header file with function declarations and macros |
| `Makefile`   | Build automation (optional) |
| `README.md`  | Project documentation |

```

## Usage

## Encoding

./stego -e beautiful.bmp secret.txt stego.bmp
beautiful.bmp : Original image

secret.txt : File to be hidden

stego.bmp : Stego image with hidden data

## Decoding

./stego -d stego.bmp decode.txt
stego.bmp : Stego image


## Technologies Used

```
- C Programming Language
- GCC Compiler
- File Handling (Binary I/O)
- Bitwise Operations
- BMP Image Format
- Linux Environment
- Git & GitHub

``` 
decode.txt : Extracted secret file

