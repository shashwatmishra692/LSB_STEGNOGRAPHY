#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "data_types.h"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    int dec_ext_size;
    char *dec_ext;
    char *dec_file_name;

    long int size_secret_file;
    char *dec_magic_str;
    FILE *fptr_decode_file;
    char img_data[8];
    char decode_data;

}DecodeInfo;

typedef enum{
    e_success_1,
    e_failure_1,
}Status_decode;

Status_decode read_and_validate_decode_args(char*argv[],DecodeInfo *decinfo);

Status_decode decode_secret_filesize(DecodeInfo * decinfo);

Status_decode open_files_for_decoding(DecodeInfo * decinfo);

Status_decode get_decoded_file_extension_size(DecodeInfo * decinfo);

Status_decode decode_and_check_magic_string(DecodeInfo * decinfo);

Status_decode decode_img_lsb(char*enc_data,char*dec_data);

Status_decode get_decoded_file_extension(DecodeInfo *decInfo);

Status_decode decode_remaining_data(DecodeInfo*decInfo);

Status_decode do_decoding(DecodeInfo *decInfo);

#endif