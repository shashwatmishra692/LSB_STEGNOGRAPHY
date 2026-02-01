#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode_001.h"
#include "decode_001.h"
#include "data_types.h"

int main(int argc,char*argv){
    if(argv[1]!=NULL && check_operation_type(argv)==e_encode){
        printf("INFO ::: Encoding Selected\n");
        EncodeInfo encinfo;
        if(read_and_validate_encode_args(argv,&encinfo)==e_success){
            printf("INFO ::: Read and Validate Successful\n");
            if(do_encoding(&encinfo)==e_success){
                printf("INFO ::: Encoding Successfully Completed\n");
            }
            else{
                printf("INFO ::: Encoding Unsuccessful\n");
            }
        }
        else{
            printf("INFO ::: Read and Validate Unsuccessful\n");
        }
    }
    else if(argv[1]!=NULL && check_operation_type(argv)==e_decode){
        printf("INFO ::: Decoding Selected\n");
        DecodeInfo decinfo;
        if(read_and_validate_decode_args(argv,&decinfo)==e_success_1){
            printf("INFO ::: Read and Validate Successfull\n");
            if(do_decoding(&decinfo)==e_success_1){
                printf("INFO ::: Decoding Successfully Completed\n");
            }
            else{
                printf("INFO ::: Decoding Unsuccessful\n");
            }
        }
        else{
            printf("INFO ::: Read and Validate Unsuccessful\n");
        }
    }
    else{
        printf("INFO ::: Invalid Input\n");
    }
}

OperationType check_operation_type(char*argv[]){
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char*argv[],EncodeInfo*encInfo){
    if(argv[2]!=NULL && strstr(argv[2],".bmp")!=NULL){
        encInfo->src_image_fname=argv[2];
    }
    else{
        return e_failure;
    }
    
    //check .txt or not
    if(argv[3]!=NULL && strstr(argv[3],".txt")!=NULL){
        encInfo->secret_fname=argv[3];
    }
    else{
        return e_failure;
    }


    if(argv[4]==NULL){
        encInfo->stego_image_fname="stego.bmp";
    }
    else{
        encInfo->stego_image_fname=argv[4];
    }
    return e_success;
}

Status_decode read_and_validate_decode_args(char*argv[],DecodeInfo *decinfo){
    char*ext=strrchr(argv[2],'.');
    if(!ext || strcmp(ext,".bmp")!=0){
        return e_failure_1;
    }

    decinfo->stego_image_fname=malloc(strlen(argv[2])+1);
    strcpy(decinfo->stego_image_fname,argv[2]);

    /*Check output file name*/
    if(argv < 4){
        decinfo->dec_file_name=malloc(strlen("output")+1);
        strcpy(decinfo->dec_file_name,"output");
    }
    else{
        decinfo->dec_file_name=malloc(strlen(argv[3])+1);
        strcpy(decinfo->dec_file_name,argv[3]);
    }
    return e_success_1;
}