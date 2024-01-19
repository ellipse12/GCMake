#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include "glob.h"
#include "cJSON.h"
typedef struct {
    const char** files;
    size_t files_length;
    const char** dirs;
    size_t dirs_length;
    const char** file_patterns;
    size_t file_patterns_length;
    const char** dirs_patterns;
    size_t dirs_patterns_length;

    cJSON* json; 

}Data;


const char* gcmake_read(){
    FILE* fp;
    if(fopen_s(&fp, "GCMake.json", "r+") != 0){
        perror("Could not find GCMake.json file.");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    rewind(fp);
    char* out = malloc(sizeof(char) * length);
    if(out == NULL){
        perror("Could not allocate memory for file data, either filesize is too large or the program has run out of memory");
        exit(1);
    }
    fread(out,sizeof(char), length, fp); 
    if(ferror(fp) != 0){
        perror("Could not read file");
        exit(1);
    }
    return out;

}

size_t iterate_json(cJSON* jsonObject,const char* str, char** out){
    cJSON* jsonArray = cJSON_GetObjectItemCaseSensitive(jsonObject, str);
    if(cJSON_IsArray(jsonArray)){
        size_t arrsize = cJSON_GetArraySize(jsonArray);
        if(out !=NULL){
            int i = 0;
            cJSON* item;
            cJSON_ArrayForEach(item, jsonArray){
                out[i++] = item->valuestring;
            }
        }
        return arrsize;

    }
    return -1;
}

void gcmake_parse(Data* out, const char* data){
    cJSON* json = cJSON_Parse(data);
    if(json == NULL){
        const char* error = cJSON_GetErrorPtr();
        printf("Error! Malformed JSON\n");
        cJSON_Delete(json);
        exit(1);
    }
    out->json = json;
    out->file_patterns_length = iterate_json(json, "files", NULL);
    if(out->file_patterns_length == -1){
        printf("Could not find \"files\" parameter in GCMake.json, cannot continue...");
        exit(1);
        
    }
    out->file_patterns = malloc(sizeof(char) * out->file_patterns_length);
    iterate_json(json, "files", out->file_patterns);

}


int main(){

    const char* data = gcmake_read();
    
    Data da = {0};
    gcmake_parse(&da, data);
     
    return 0;
}
