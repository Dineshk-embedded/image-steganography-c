#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Validate decode arguments
 * usage: ./a.out -d <stego.bmp> [output file]
 * If no output name is given, "output" + decoded extension is used. */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *dot = strrchr(argv[2], '.');
    if (dot != NULL && strcmp(dot, ".bmp") == 0)
        decInfo->stego_image_fname = argv[2];
    else
        return e_failure;

    if (argv[3] != NULL)
        decInfo->output_fname = argv[3];
    else
        decInfo->output_fname = "output.txt";

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char img_buffer[8];
    char data;
    int len = strlen(magic_string);
    char decoded_magic[len + 1];

    /* pixel data starts after the 54 byte BMP header */
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    for (int i = 0; i < len; i++)
    {
        if (fread(img_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
            return e_failure;
        decode_byte_from_lsb(&data, img_buffer);
        decoded_magic[i] = data;
    }
    decoded_magic[len] = '\0';

    if (strcmp(decoded_magic, magic_string) == 0)
    {
        printf("Decoding Magic String            : Success\n");
        return e_success;
    }
    return e_failure;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char img_buffer[32];

    if (fread(img_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
        return e_failure;
    decode_size_from_lsb(&decInfo->size_secret_file_extn, img_buffer);

    /* extn_secret_file[] holds 5 chars (4 + '\0'); reject garbage values */
    if (decInfo->size_secret_file_extn < 1 || decInfo->size_secret_file_extn > 4)
        return e_failure;

    printf("Decoding secret file extn size   : Success\n");
    return e_success;
}

Status decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo)
{
    char img_buffer[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(img_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
            return e_failure;
        decode_byte_from_lsb(&file_extn[i], img_buffer);
    }
    file_extn[size] = '\0';

    printf("Decoding secret file extn        : Success\n");
    return e_success;
}

Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    char img_buffer[32];

    if (fread(img_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
        return e_failure;
    decode_size_from_lsb(file_size, img_buffer);   /* store into caller's variable */

    printf("Decoding secret file Size        : Success\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char img_buffer[8];
    char data;

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(img_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
            return e_failure;
        decode_byte_from_lsb(&data, img_buffer);
        fwrite(&data, 1, 1, decInfo->fptr_output);
    }

    printf("Decoding secret file Data        : Success\n");
    return e_success;
}

Status decode_byte_from_lsb(char *data, char *imageBuffer)
{
    unsigned char value = 0;
    for (int i = 0; i < 8; i++)
        value |= (imageBuffer[i] & 1) << i;
    *data = value;
    return e_success;
}

Status decode_size_from_lsb(int *size, char *imageBuffer)
{
    unsigned int value = 0;
    for (int i = 0; i < 32; i++)
        value |= (unsigned int)(imageBuffer[i] & 1) << i;
    *size = (int)value;
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("=============================\n");
    printf("           DECODING           \n");
    printf("=============================\n");

    if (open_decode_files(decInfo) != e_success)
        return e_failure;
    printf("Stego Image      : %s \n", decInfo->stego_image_fname);
    printf("Output File      : %s \n\n", decInfo->output_fname);


    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
    {
        printf("Decoding Magic String            : Failure (not a stego image?)\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    if (decode_secret_file_extn_size(decInfo) != e_success)
    {
        printf("Decoding secret file extn size   : Failure\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo->extn_secret_file,
                                decInfo->size_secret_file_extn, decInfo) != e_success)
    {
        printf("Decoding secret file extn        : Failure\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    /* no output name given -> "output" + decoded extension */
    if (decInfo->output_fname == NULL)
    {
        snprintf(decInfo->output_buf, sizeof(decInfo->output_buf),
                 "output%s", decInfo->extn_secret_file);
        decInfo->output_fname = decInfo->output_buf;
    }

    if (decode_secret_file_size(&decInfo->size_secret_file, decInfo) != e_success
        || decInfo->size_secret_file < 0)
    {
        printf("Decoding secret file Size        : Failure\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    if (open_output_file(decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) != e_success)
    {
        printf("Decoding secret file Data        : Failure\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);      /* flushes the secret file to disk */
    return e_success;
}
