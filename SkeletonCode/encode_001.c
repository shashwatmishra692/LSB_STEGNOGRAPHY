#include <stdio.h>
#include "encode_001.h"
#include "data_types.h"
#include "common.h"
#include <string.h>

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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
uint get_file_size(FILE*secret){
    fseek(secret,0,SEEK_END);
    return ftell(secret);
}
Status check_capacity(EncodeInfo*encInfo){
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (54+16+32+32+32+(encInfo->size_secret_file * 8))){
        return e_success;
    }
    else{
        return e_failure;
    }
}

Status copy_bmp_header(FILE*src,FILE*output){
    char str[54];
    rewind(src);
    fread(str,54,sizeof(char),src);
    fwrite(str,54,sizeof(char),output);
    return e_success;
}

Status encode_byte_to_lsb(char data,char*image_data){
    char ch=data;

    for(int i=7;i>=0;i--){
        char bit = ch & 1;
        ch >>=1;
        image_data[i] = (image_data[i] & 0XFE)|bit;
    }
    return e_success;
}

Status encode_data_to_image(const char*data,int size,FILE*fptr_src,FILE*fptr_stego,EncodeInfo*encInfo){
    for(int i=0;i<size;i++){
        fread(encInfo->image_data,8,sizeof(char),fptr_src);
        encode_byte_to_lsb(data[i],encInfo->image_data);
        fwrite(encInfo->image_data,8,sizeof(char),fptr_stego);
    }
}

Status encode_magic_string(const char*magic_str,EncodeInfo * encinfo){
    encode_data_to_image(magic_str,strlen(magic_str),encinfo->fptr_src_image,encinfo->fptr_stego_image,encinfo);
    return e_success;
}

Status encode_size_to_lsb(char*image_buffer,int size){
    unsigned int num=size;
    for(int i=31;i>=0;i--){
        int bit = num & 1;
        num >>= 1;

        image_buffer[i] = (image_buffer[i] & 0XFE)|bit;
    }
    return e_success;
}

Status encode_size(int size,FILE*src_image,FILE*stego_image){
    char str[32];
    fread(str,32,sizeof(char),src_image);
    encode_size_to_lsb(str,size);
    fwrite(str,32,sizeof(char),stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char*file_extn,EncodeInfo * encinfo){
    file_extn=".txt";
    encode_data_to_image(file_extn,strlen(file_extn),encinfo->fptr_src_image,encinfo->fptr_stego_image,encinfo);
    return e_success;
}

Status encode_secret_file_size(long int file_size,EncodeInfo * encInfo){
    char str[32];
    fread(str,32,sizeof(char),encInfo->fptr_src_image);
    encode_size_to_lsb(str,file_size);
    fwrite(str,32,sizeof(char),encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo * encInfo){
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char ch;
    for(int i=0;i<encInfo->size_secret_file;i++){
        fread(encInfo->image_data,8,sizeof(char),encInfo->fptr_src_image);
        fread(&ch,1,sizeof(char),encInfo->fptr_secret);
        encode_byte_to_lsb(ch,encInfo->image_data);
        fwrite(encInfo->image_data,8,sizeof(char),encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE*fptr_src,FILE*fptr_stego){
    char ch;
    while(fread(&ch,1,sizeof(char),fptr_src)>0){
        fwrite(&ch,1,sizeof(char),fptr_stego);
    }
    return e_success;
}

Status do_encoding(EncodeInfo*encInfo){
    if(open_files(encInfo)==e_success){
        printf("Files opened successfully\n");
        if(check_capacity(encInfo)==e_success){
            printf("INFO ::: Image has enough capacity\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
                printf("INFO ::: 54 Bytes Copied Successfullty\n");
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success){
                    printf("INFO ::: Encode Magic String Successful\n");
                    if(encode_size(strlen(".txt"),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
                        printf("INFO ::: Extension Size encoded Succesfully\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success){
                            printf("INFO ::: Secret File extension Successful\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success){
                                printf("INFO ::: Secret File Size Encoded Successfully\n");
                                if(encode_secret_file_data(encInfo)==e_success){
                                    printf("INFO ::: Secret data encoded successfully\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
                                        printf("INFO ::: Remaining data copied successfully\n");
                                    }
                                    else{
                                        printf("INFO ::: Copying leftover data unsuccessfull\n");
                                        return e_failure;
                                    }
                                }
                                else{
                                    printf("INFO ::: Secret data encoded unsuccessfull\n");
                                    return e_failure;
                                }
                            }
                            else{
                                printf("INFO ::: Secret File Size encoded unsuccessfull\n");
                                return e_failure;
                            }
                        }
                        else{
                            printf("INFO ::: Secret File extension Unsuccessful\n");
                            return e_failure;
                        }
                    }
                    else{
                        printf("INFO ::: Extension Size encoded unsuccessfull\n");
                        return e_failure;
                    }
                }
                else{
                    printf("INFO ::: Encode Magic String not Successful\n");
                    return e_failure;
                }
            }
            else{
                printf("INFO ::: Header not copied Successfully\n");
            }
        }
        else{
            printf("INFO ::: Image does not have enough capacity\n");
            return e_failure;
        }
    }
    else{
        printf("INFO ::: Files opened unsuccesfull\n");
        return e_failure;
    }
    return e_success;
}