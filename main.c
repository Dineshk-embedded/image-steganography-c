/*
NAME: Dinesh Kumar S 
class: 26012B
Start Date : 03.09.2026
End Date : 24.09.2026
DESCRIPTION: 
Image Steganography is a technique used to securely hide secret information inside an image without visibly changing its appearance. 
In this project, a secret file is embedded into a BMP image using the Least Significant Bit (LSB) technique. 
The project is implemented in C and consists of two major blocks: Encoding and Decoding.

Encoding :
Hides a secret file inside a BMP image using the LSB (Least Significant Bit) technique. 
It encodes the magic string, secret file extension, file size, and file data into the image to generate a stego image.

Decoding :
Extracts the hidden information from the stego image using the LSB technique. 
It decodes the magic string, file extension, file size, and secret data to recreate the original file.

Sample Input (Encoding):
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


Sample Input (Decoding) :
=============================
           DECODING           
=============================
Stego Image      : stego.bmp 
Output File      : output.txt 

Decoding Magic String            : Success
Decoding secret file extn size   : Success
Decoding secret file extn        : Success
Decoding secret file Size        : Success
Decoding secret file Data        : Success


*/
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{
    //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // yes -> EncodeInfo encInfo;
            // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> print success msg 
                            // check do_encoding(&encInfo) is returning e_success or e_failure
                                    // failure -> print error msg and stop
                                    // success -> print success msg and stop


    if (argc < 2)
    {
        printf("Usage:\n  Encode: %s -e <src.bmp> <secret file> [stego.bmp]\n", argv[0]);
        printf(" Decode: %s -d <stego.bmp> [output file]\n", argv[0]);
        return 1;
    }

    OperationType op = check_operation_type(argv[1]);

    if(op == e_encode)
    {
        if (argc < 4)
        {
            printf("Usage: %s -e <src.bmp> <secret file> [stego.bmp]\n", argv[0]);
            return 1;
        }
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo) != e_success)
        {
            printf("arguments are wrong\n");
            return e_failure;
        }
        
        if (do_encoding(&encInfo) != e_success)
        {
            printf("Encoding failed\n");
            return 1;
        }

    }
    else if(op == e_decode)
    {
        if (argc < 3)
        {
            printf("Usage: %s -d <stego.bmp> [output file]\n", argv[0]);
            return 1;
        }

        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) != e_success)
        {
            printf("arguments are wrong\n");
            return 1;
        }
        if (do_decoding(&decInfo) != e_success)
        {
            printf("Decoding failed\n");
            return 1;
        }
    }
    else
    {
        printf("Invalid Operation\n");
        return e_failure;
    }
}

OperationType check_operation_type(char *symbol)
{
    //step 1 -> check symbol is -e or not
        // yes -> return e_encode
    
    // step 2 -> check symbol is -d or not
        // yes -> return e_decode

    // return e_unsupported

    if(strcmp(symbol,"-e") == 0)
    return e_encode;

    if(strcmp(symbol,"-d") == 0)
    return e_decode;

    return e_unsupported;

}
