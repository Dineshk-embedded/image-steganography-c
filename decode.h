#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

/* Structure to store information required for decoding */

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;//Stores Name of image containing the hidden data.
    FILE *fptr_stego_image; //Stores the file pointer returned by the file pointer

    /* Secret File Info */
    char extn_secret_file[5];//This Stores the extension we extract from the image
    int size_secret_file; //This Stores the size of hidden secret file

    /* Output File Info */
    char *output_fname; //Stores the name of the file we are going to create
    FILE *fptr_output; //Stores File pointer to the output file
    int size_secret_file_extn;
    char output_buf[100];   //builds "output" + extension when no name is given

} DecodeInfo;

/* Decoding function prototypes */

/* Read and validate Decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get file pointer for input and output files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data */
Status decode_byte_from_lsb(char *data, char *imageBuffer);

/* Decode a size from LSB of image data */
Status decode_size_from_lsb(int *size, char *imageBuffer);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

/* Decode Secret File Extension Size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret File Extension */
Status decode_secret_file_extn(char *file_extn,int size,DecodeInfo *decInfo);

/* Decode Secret File Size */
Status decode_secret_file_size(int *file_size,DecodeInfo *decInfo);

/*Open Output File*/
Status open_output_file(DecodeInfo *decInfo);

/* Decode Secret File Data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif