/*
Name :Anil P Kare
Date :12/11/2023
File name :Test_encode.c  (Main function)
*/


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main( int argc, char *argv[])
{
    //declaring the structure for encode and decode operation
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(argc < 3)
    {
	printf("Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\nDecoding : ./a.out -d <.bmp file> [output file]\n");
	return -1;
    }

    if (check_operation_type(argv) == e_encode )
    {
	if (argc < 4)
	{
	    printf("Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
	    return -1;
	}
	if (read_and_validate_encode_args(argv, &encInfo) == e_success)
	{
	    if (do_encoding(&encInfo) == e_success)
	    {
		printf("INFO: ## Encoding Done Successfully ##\n");
	    }
	    else
	    {
		printf("Encoding is a failure\n");
	    }
	}
	else
	{
	    printf("Read and validate was not success\n");
	    return 1;
	}
    }
    else if ( check_operation_type(argv)  == e_decode )
    {
	if (argc < 3)
	{
	    printf("Decoding : ./a.out -d <.bmp file> [output file]\n");
	    return -1;
	}
	if (read_and_validate_decode_argv(argv, &decInfo) == d_success)
	{
	    if(do_decoding(&decInfo) == d_success)
	    {
		printf("INFO: ## Decoding Done Successfully ##\n");
	    }
	}
	else
	{
	    printf("Read and validate was failure\n");
	    return 1;
	}
    }
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}
