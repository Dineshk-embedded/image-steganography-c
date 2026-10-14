#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\t", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);


    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    // return secret file size

    fseek(fptr,0,SEEK_END);
    uint size = ftell(fptr);

    rewind(fptr);

    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step 1 -> check argv[2] is having .bmp or not
        //yes -> store the file name into encInfo -> src_image_fname = argv[2]
        //no - > return e_failure

    // step 2 -> check argv[3] is having extn there or not
        // yes - > store the file name into encInfo -> secret_fname = argv[3]
        // no -> return e_failure
    
    // step 3 -> check argv[4] is NULL or not
        //yes -> check argv[4] is having .bmp or not
                //yes -> store the file name into encInfo -> stego_image_fname = argv[4]
                //no - > return e_failure
        
        //no -> store default name the encInfo -> stego_image_fname = "stego.bmp"


    // return e_success;

    char *dot;

    /* source image must be .bmp */
    dot = strrchr(argv[2], '.');
    if (dot != NULL && strcmp(dot, ".bmp") == 0)
        encInfo->src_image_fname = argv[2];
    else
        return e_failure;

    /* secret file needs an extension; store it (e.g. ".txt") for encoding */
    dot = strrchr(argv[3], '.');
    if (dot != NULL && strlen(dot) < sizeof(encInfo->extn_secret_file))
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, dot);
    }
    else
        return e_failure;

    /* optional output name */
    if (argv[4] != NULL)
    {
        dot = strrchr(argv[4], '.');
        if (dot != NULL && strcmp(dot, ".bmp") == 0)
            encInfo->stego_image_fname = argv[4];
        else
            return e_failure;
    }
    else
        encInfo->stego_image_fname = "stego.bmp";

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //step 1 -> encInfo -> image_capacity = call get_image_size_for_bmp(encoInfo -> fptr_src_image)
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //step 2 -> encInfo -> size_secret_file = call get_file_size(encInfo -> fptr_secret)
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //step 3 -> check encInfo -> image_capacity > 16 + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + (encInfo -> size_secret_file * 8)
                //t -> return e_success;
                //f -> return e_failure

    if((long)(encInfo -> image_capacity) > (long)(16 + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + (encInfo -> size_secret_file * 8)))
    return e_success;
    else
    return e_failure;

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // step 1 -> rewind source file pointer

    // step 2 -> read 54 bytes from source file

    // step 3 -> write 54 bytes to dest file

    // return e_success
    
    // printf("Before fread src = %ld\n", ftell(fptr_src_image));
    rewind(fptr_src_image);
    // printf("After fread src = %ld\n", ftell(fptr_src_image));
    char buffer[54];
    if(fread(buffer,1,54,fptr_src_image) != 54)
    {
        printf("Error on fread\n");
        return e_failure;
    }
    // rewind(fptr_src_image);

    if(fwrite(buffer,1,54,fptr_dest_image) != 54)
    {
        printf("Error on fwrite\n");
        return e_failure;
    }
    // rewind(fptr_dest_image);

    printf("Copy Bmp Header                  : Success\n");
    return e_success;


}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char buffer[8];

    //step 1 -> read 8 bytes from source file

    //step 2 -> call encode_byte_to_lsb(magic_string[i], buffer)

    //step 3 -> write the buffer into dest file

    // repeat this for strlen(magic_string) times from step 1


    char image_buffer[8];

    int size = strlen(magic_string);

    for(int i = 0; i < size; i++)
    {
        fread(image_buffer,8,1,encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i],image_buffer);

        fwrite(image_buffer,8,1,encInfo->fptr_stego_image);
    }

    printf("Encoding Magic String            : Success\n");
    return e_success;
    
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // char buffer[32];

    //step 1 -> read 32 bytes from source file

    //step 2 -> call encode_size_to_lsb(size, buffer)

    //step 3 -> write the buffer into dest file

    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    printf("Encoding secret file extn size   : Success\n");
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // char buffer[8];

    //step 1 -> read 8 bytes from source file

    //step 2 -> call encode_byte_to_lsb(file_extn[i], buffer)

    //step 3 -> write the buffer into dest file

    // repeat this for strlen(file_extn) times from step 1   

    char buffer[8];
    int extn_size = strlen(file_extn);

    for(int i = 0; i < extn_size; i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    } 

    printf("Encoding secret file extn        : Success\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // char buffer[32];

    //step 1 -> read 32 bytes from source file

    //step 2 -> call encode_size_to_lsb(file_size, buffer)

    //step 3 -> write the buffer into dest file

    char buffer[32];

    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);

    printf("Encoding secret file Size        : Success\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //step 1 -> read secret data from file and store it into encInfo -> secret_data

    // char buffer[8];

    //step 2 -> read 8 bytes from source file.

    //step 3 -> call encode_byte_to_lsb(encInfo -> secret_data[i], buffer)

    //step 4 -> write the buffer into dest file

    // repeat this for strlen(encInfo -> size_secret_file) times from step 2

    // char buffer[8];
    // fread(encInfo->secret_data,1,encInfo->size_secret_file,encInfo->fptr_secret);

    // for(int i = 0; i < encInfo->size_secret_file; i++)
    // {
    //     fread(buffer,8,1,encInfo->fptr_src_image);
    //     encode_byte_to_lsb(encInfo->secret_data[i], buffer);
    //     fwrite(buffer,8,1,encInfo->fptr_stego_image);
    // }

    char buffer[8];
    char ch;

    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(&ch, 1, 1, encInfo->fptr_secret);

        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    printf("Encoding secret file Data        : Success\n");
    return e_success;

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // write logic for copy remaining data

    // return e_success

    // char ch
    // while (fread(&ch , 1 ,1 ,fptr_src) == 1)
    // fwrite(&ch, 1 ,1 ,fptr_dest);

    char buf[4096];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), fptr_src)) > 0)
    {
        if (fwrite(buf, 1, n, fptr_dest) != n)
            return e_failure;
    }

    printf("Copy Remaining img data          : Success\n");
    return e_success;
}

Status encode_byte_to_lsb(char in_data, char *image_buffer)
{
    // write logic to encode the char

    // repeat for 8 bits.
    // Extract LSB from data.
    // clean LSB of image byte.
    //put data bit into image LSB.
    // shift data right.
    
    unsigned char data = in_data;

    for(int i = 0; i < 8; i++)
    {
        char data_bit = data & 1;
        image_buffer[i] = image_buffer[i] & ~1 ;
        image_buffer[i] = image_buffer[i] | data_bit ;
        data = data >> 1;
    }

    return e_success;
}

Status encode_size_to_lsb(long in_size, char *imageBuffer)
{
    unsigned long size = in_size;

    // write logic to encode the int

    for(int i = 0; i < 32; i++)
    {
        int size_bit = size & 1;
        imageBuffer[i] = imageBuffer[i] & ~1;
        imageBuffer[i] = imageBuffer[i] | size_bit;

        size = size >> 1;
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //step 1 -> check open_files(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 2 -> check check_capacity(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 3 -> call copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 4 -> call encode_magic_string(MAGIC_STRING, encInfo)

    //step 5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)

    //step 6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)

    //step 7 -> call encode_secret_file_size(encInfo ->  size_secret_file, encInfo)

    //step 8 -> call encode_secret_file_data(encInfo)

    //step 9 -> copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success


    // return e_success.

    printf("=============================\n");
    printf("           ENCODING           \n");
    printf("=============================\n");
    if(open_files(encInfo) == e_success)
    {
        printf("Source Image     : %s \n",encInfo->src_image_fname);
        printf("Secret File      : %s \n",encInfo->secret_fname);
        printf("Stego Image      : %s \n\n",encInfo->stego_image_fname);
    }
    else
    {
        printf("Error open files\n");
        return e_failure;
    }

    if(check_capacity(encInfo) != e_success)
    {
        printf("Error check capacity\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success)
    {
        printf("Copy bmp Header                  : Failure\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING,encInfo) != e_success)
    {
        printf("Encoding Magic String            : Failure\n");
        return e_failure;
    }
    
    if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) != e_success)
    {
        printf("Encoding secret file extn size   : Failure\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) != e_success)
    {
        printf("Encoding secret file extn        : Failure\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo ->  size_secret_file, encInfo) != e_success)
    {
        printf("Encoding secret file Size        : Failure\n");
        return e_failure;
    }
    
    if(encode_secret_file_data(encInfo) != e_success)
    {
        printf("Encoding secret file Data        : Failure\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
    {
        printf("Copy Remaining img data          : Failure\n");
        return e_failure;
    }

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);   /* make sure everything is flushed */

    return e_success;

}