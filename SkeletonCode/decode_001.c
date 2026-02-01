#include <stdio.h>
#include "decode_001.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

Status_decode decode_img_lsb(char*enc_data,char*dec_data){
    unsigned char mask=1<<7;
    for(int i=0;i<8;i++){
        *dec_data=*dec_data|((enc_data[i] & 0x1)<<(7-i));
    }
    return e_success_1;
}

Status_decode decode_secret_filesize(DecodeInfo * decinfo){
    long int secret_size=0;
    char ch=0;
    for(int i=0;i<4;i++){
        fread(decinfo->img_data,8,sizeof(char),decinfo->fptr_stego_image);
        decode_img_lsb(decinfo->img_data,&ch);
        secret_size = (secret_size | ch);
        secret_size = ((secret_size) << (8*(3-i)));
        ch=0;
    }
    decinfo->size_secret_file=secret_size;
    printf("Secret File Size: %d\n",decinfo->size_secret_file);
    return e_success_1;
}

Status_decode open_files_for_decoding(DecodeInfo * decinfo){
    decinfo->fptr_stego_image=fopen(decinfo->stego_image_fname,"r");

    if(decinfo->fptr_stego_image == NULL){
        perror("open");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decinfo->stego_image_fname);
        return e_failure_1;
    }

    return e_success_1;
}

Status_decode get_decoded_file_extension_size(DecodeInfo * decinfo){
    int ext_size=0;

    for(int i=0;i<4;i++){
        char ch=0;
        fread(decinfo->img_data,8,sizeof(char),decinfo->fptr_stego_image);
        decode_img_lsb(decinfo->img_data,&ch);
        ext_size=(ext_size|ch);
        ch=0;
        ext_size=(ext_size) << (8*(3-i));
    }
    decinfo->dec_ext_size=ext_size;
    printf("Extension size = %d\n",decinfo->dec_ext_size);
    return e_success_1;
}

Status_decode decode_and_check_magic_string(DecodeInfo * decinfo){
    //Skip the first 54 bytes
    fseek(decinfo->fptr_stego_image,54,SEEK_SET);
    char ch=0;
    char str[3];
    decinfo->dec_magic_str = malloc(sizeof(char)*3);

    for(int i=0;i<2;i++){
        fread(decinfo->img_data,8,sizeof(char),decinfo->fptr_stego_image);
        decode_img_lsb(decinfo->img_data,&ch);
        str[i]=ch;
        ch=0;
    }
    str[2]='\0';

    strcpy(decinfo->dec_magic_str,str);
    decinfo->dec_magic_str[2]='\0';
    printf("MAGIC: %s\n",decinfo->dec_magic_str);

    if(strcmp(MAGIC_STRING,decinfo->dec_magic_str)==0){
        return e_success_1;
    }
    else{
        return e_failure_1;
    }
}

Status_decode get_decoded_file_extension(DecodeInfo *decInfo){
    decInfo->dec_ext=malloc(sizeof(char)*(decInfo->dec_ext_size + 1));
    for(int i=0; i<decInfo->dec_ext_size;i++){
        char ch=0;
        fread(decInfo->img_data,8,sizeof(char),decInfo->fptr_stego_image);
        decInfo->dec_ext[i]=ch;
        ch=0;
    }
    decInfo->dec_ext[decInfo->dec_ext_size]='\0';

    strcat(decInfo->dec_file_name,decInfo->dec_ext);
    decInfo->fptr_decode_file=fopen(decInfo->dec_file_name,"w");
    if(decInfo->fptr_decode_file==NULL){
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->fptr_decode_file);
        return e_failure_1;
    }
    printf("INFO ::: Decode file %s successfully created\n",decInfo->dec_file_name);
    return e_success_1;
}

Status_decode decode_remaining_data(DecodeInfo*decInfo){
    char ch=0;
    for(int i=0;i<decInfo->size_secret_file;i++){
        fread(decInfo->img_data,8,sizeof(char),decInfo->fptr_stego_image);
        decode_img_lsb(decInfo->img_data,&ch);
        fwrite(&ch,1,sizeof(char),decInfo->fptr_decode_file);
        ch=0;
    }
    return e_success_1;
}

Status_decode do_decoding(DecodeInfo *decInfo){
    if(open_files_for_decoding(decInfo)==e_success_1){
        printf("INFO ::: Files opened successfully for decoding\n");
        if(decode_and_check_magic_string(decInfo)==e_success_1){
            printf("Magic string decoded and it is valid\n");
            if(get_decoded_file_extension_size(decInfo)==e_success_1){
                printf("INFO ::: Decode file extension size successfull\n");
                if(get_decoded_file_extension(decInfo)==e_success_1){
                    printf("INFO ::: Decode file extension extracted\n");
                    if(decode_secret_filesize(decInfo)==e_success_1){
                        printf("INFO ::: Decode secret file size successfull\n");
                        if(decode_remaining_data(decInfo)==e_success_1){
                            printf("INFO ::: Decode successful\n");
                            fclose(decInfo->fptr_decode_file);
                            fclose(decInfo->fptr_stego_image);
                        }
                        else{
                            printf("INFO ::: Decode Failed\n");
                        }
                    }
                    else{
                        printf("INFO ::: Decode Secret File Size Failed\n");
                    }
                }
                else{
                    printf("INFO ::: Decode file extension failed\n");
                }
            }
            else{
                printf("INFO ::: Decode file extension size failed\n");
            }
        }
        else{
            printf("INFO ::: Magic String validation failed\n");
            return e_failure_1;
        }
    }
    else{
        printf("INFO ::: Failed to open files for deoding\n");
        return e_failure_1;
    }
    return e_success_1;
}