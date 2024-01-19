#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
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
    if((fp = fopen("GCMake.json", "r+")) == NULL){
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

size_t iterate_json(cJSON* jsonObject,const char* str, const char** out){
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
        printf("Could not find required parameter: \"files\", in GCMake.json.");
        exit(1);
        
    }
    out->file_patterns = malloc(sizeof(char) * out->file_patterns_length);
    iterate_json(json, "files", out->file_patterns);
    
    out->dirs_patterns_length = iterate_json(json, "dirs", NULL);
    if(out->dirs_patterns_length == -1){
        printf("Could not find required parameter: \"dirs\", in GCMake.json.");
        exit(1);
    }
    out->dirs_patterns = malloc(sizeof(char) * out->dirs_patterns_length);
    iterate_json(json, "dirs", out->dirs_patterns);
}

int gcmake_visit(const char* path, const struct stat* stats, int type, struct FTW* ftwbuf){
	printf("%s\n", path);
	return type==FTW_D ? FTW_SKIP_SUBTREE : FTW_CONTINUE;
}

void gcmake_run(Data* data){
	if(nftw(".", gcmake_visit, 10, FTW_ACTIONRETVAL) == -1){
		perror("nftw");
		exit(EXIT_FAILURE);
	}
}


int main(){

    const char* data = gcmake_read();
    
    Data da = {0};
    gcmake_parse(&da, data);
    for(int i = 0; i < da.file_patterns_length; i++){
        printf("pattern: %s\n", da.file_patterns[i]);
    }     
	gcmake_run(&da);
    return 0;
}
