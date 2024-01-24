#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ftw.h>
#include <glob.h>
#include "cJSON.h"
#include <assert.h>
typedef struct {
    const char** files;
    size_t files_length;
    const char** dirs;
    size_t dirs_length;
    const char** file_patterns;
    size_t file_patterns_length;
    const char** dirs_patterns;
    size_t dirs_patterns_length;
	char* out_buffer;
    size_t out_buffer_length;
	cJSON* json; 

}Data;

Data data = {0};


void gcmake_write(){
	FILE* fp;
	char* buffer;
	if((fp = fopen("GCMakeLists.txt", "r")) == NULL){
		perror("Could not find or create CMakeList.txt file.");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);
	rewind(fp);
	char* temp = malloc(sizeof(char) * length + 1);
	fread(temp, sizeof(char), length, fp);
	if(ferror(fp) != 0){
		perror("An error occured!");
		fclose(fp);
		exit(1);
	}
	fclose(fp);
	if((fp = fopen("CMakeLists.txt", "w")) == NULL){
		perror("Failed to write to file");
		exit(1);
	}
	const char header[] = "########GCMake Generated Block DO NOT EDIT########\n";
	const char footer[] = "##################################################\n";
	fwrite(header, sizeof(char), sizeof(header) - 1, fp);
	fwrite("set(GCMAKE_FILES ", sizeof(char), 17, fp);
	fwrite(data.out_buffer,sizeof(char), data.out_buffer_length, fp);
	fwrite(")\n", sizeof(char), 2, fp);
	fwrite(footer, sizeof(char), sizeof(footer) - 1, fp);
	fwrite(temp, sizeof(char), length, fp);

	fclose(fp);
}

const char* gcmake_read(){
    FILE* fp;
    if((fp = fopen("GCMake.json", "r")) == NULL){
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
		fclose(fp);
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

bool glob_arr_contains(const char** arr, int arr_length, const char* value){
	for(int i = 0; i < arr_length; i++){
		if(glob(arr[i], value)){
			return true;
		}
	}
	return false;
}

int gcmake_visit(const char* path, const struct stat* stats, int type, struct FTW* ftwbuf){
    int base = ftwbuf->base;
	if(ftwbuf->level == 0){
		return FTW_CONTINUE;
	}
	if(type == FTW_D && !glob_arr_contains(data.dirs_patterns, data.dirs_patterns_length, path + base)){
		return FTW_SKIP_SUBTREE;
	}
	if(type == FTW_F && glob_arr_contains(data.file_patterns, data.file_patterns_length, path + base)){
		size_t str_len = strlen(path);
		char* temp_path = malloc(sizeof(char) * (str_len + 4));
		temp_path[0] = ' ';
		temp_path[1] = '"';
		strcpy(temp_path + 2, path);
		temp_path[str_len + 2] = '\"';
		temp_path[str_len + 3] = '\0';
		data.out_buffer = realloc(data.out_buffer, sizeof(char) * (data.out_buffer_length + str_len + 4));
		strcat(data.out_buffer, temp_path);
		data.out_buffer_length += str_len + 3;
		free(temp_path);
		
	}
	return FTW_CONTINUE;	
		

}


void gcmake_run(Data* data){
	if(nftw(".", gcmake_visit, 10, FTW_ACTIONRETVAL | FTW_CHDIR) == -1){
		perror("nftw");
		exit(EXIT_FAILURE);
	}
}

int main(){

    const char* da = gcmake_read();
	gcmake_parse(&data, da);
	gcmake_run(&data);
	gcmake_write();	


	return 0;
}
