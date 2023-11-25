/*
Name:Anil P Kare
Date:12/11/2023
File Name:Encode.c
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Function Definitions*/

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    //Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    //Read the width 
    fread(&width, sizeof(int), 1, fptr_image);
    //Printf("width = %u\n", width);

    //Read the height
    fread(&height, sizeof(int), 1, fptr_image);
    //Printf("height = %u\n", height);

    //Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    //Src image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //Do error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> src_image_fname);

    //Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> secret_fname);

    //Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo -> stego_image_fname);
    printf("INFO: Done\n");

    //No failure return e_success
    return e_success;
}


/* From here i am writing*/

Status read_and_validate_encode_args( char *argv[], EncodeInfo *encInfo )
{
    if (strcmp((strstr(argv[2],".")) ,".bmp") == 0 )
    {
	encInfo -> src_image_fname = argv[2];
    }
    else
    {
	return e_failure;
    }

    if (strcmp((strstr(argv[3],".")) ,".txt") == 0 )
    {
	encInfo -> secret_fname = argv[3];
	strcpy(encInfo -> extn_secret_file, ".txt");
    }
    else
    {
	return e_failure;
    }

    if (argv[4] != NULL)
    {
	encInfo ->stego_image_fname = argv[4];
    }
    else
    {
	printf("INFO: Output file not mentioned. Creating steged_img.bmp as default\n");
	encInfo -> stego_image_fname = "steged_img.bmp";
    }
    return e_success;
}

//function defination for encoding
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
	printf("INFO: ## Encoding Procedure Started ##\n");
	//printf("open files is success\n");
	if (check_capacity(encInfo) == e_success)
	{
	    printf("INFO: Done. Found OK\n");
	    if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
	    {
		printf("INFO: Done\n");
		if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
		    printf("INFO: Done\n");
		    if (encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
		    {
			printf("INFO: Done\n");
			if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
			{
			    printf("INFO: Done\n");
			    if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
			    {
				printf("INFO: Done\n");
				if(encode_secret_file_data(encInfo) == e_success)
				{
				    printf("INFO: Done\n");
				    if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
				    {
					printf("INFO: Done\n");
				    }
				    else
				    {
					printf("Failed to copy remaining data\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("Failed to encode secret file data\n");
				    return e_failure;
				}

			    }
			    else
			    {
				printf("Encode secret file size was a failure\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("Encode secret file extenstion was failure\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("Encode secret file extenstion size was a failure\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("Magic string was not encoded\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("Couldn't copy the bmp header\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("Check capacity is a failure\n");
	    return e_failure;
	}
    }
    else
    {
	printf("open file function is a failure\n");
	return e_failure;
    }
    return e_success;
}
//function definition for check capacity
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    printf("INFO: Checking for secret.txt size\n");
    if (encInfo -> size_secret_file)
    {
	printf("INFO: Done. Not Empty\n");
    }

    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    if (encInfo -> image_capacity > (54 + (2 + 4 + 4 + 4 + encInfo -> size_secret_file) * 8))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    char str[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    int i;
    for ( i = 0 ; i < size ; i++ )
    {
	fread(buffer, 8, 1, fptr_src_image);
	encode_byte_to_lsb(data[i], buffer);
	fwrite(buffer, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;

    for ( i = 0 ; i < 8 ; i++ )
    {
	image_buffer[i] = (image_buffer[i] & 0xFE) | (((unsigned) data >> (7 - i)) & 1);
    }
    return e_success;
}

Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Extenstion size\n");
    char buffer[32];

    fread(buffer, 32, 1, encInfo -> fptr_src_image);
    encode_int_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);

    return e_success;
}

Status encode_int_to_lsb( long data, char *image_buffer)
{
    int i;

    for ( i = 0 ; i < 32 ; i++ )
    {
	image_buffer[i] =  (image_buffer[i] & 0xFE) | (((unsigned) data >> (31 - i)) & 1);
    }
    return e_success;
}

Status encode_secret_file_extn( const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encodig secret.txt File Extenstion\n");
    char buffer[8];

    int i;
    for ( i = 0 ; i < strlen(file_extn) ; i++ )
    {
	fread(buffer, 8, 1, encInfo -> fptr_src_image);
	encode_byte_to_lsb(file_extn[i], buffer);
	fwrite(buffer, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Size\n");
    char buffer[32];

    fread(buffer, 32, 1, encInfo -> fptr_src_image);
    encode_int_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Data\n");
    char buffer[8];

    fseek(encInfo -> fptr_secret, 0, SEEK_SET);

    for (int i = 0 ; i < (encInfo -> size_secret_file - 1) ; i++ )
    {
	fread(buffer, 8, 1, encInfo -> fptr_src_image);
	fread(&(encInfo -> secret_data[0]), 1, 1, encInfo -> fptr_secret);
	encode_byte_to_lsb(encInfo -> secret_data[0], buffer);
	fwrite(buffer, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while (fread(&ch, 1, 1, fptr_src_image) > 0)
    {
	fwrite(&ch, 1, 1, fptr_stego_image);
    }
    return e_success;
}

