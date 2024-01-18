#include <stdio.h>
#include <ftw.h>
#include <stdbool.h>

bool glob(const char* pat, const char* str){
    const char* back_pat = NULL;
    const char* back_str;
    while(true){
        unsigned char c = *str++;
        unsigned char d = *pat++;

        switch(d){
            case '?':
                    if(c == '\0') return false;
                    break;
            case '*':
                    if(*pat == '\0') return true;
                    back_pat = pat;
                    back_str = --str;
                    break;
            case '[': 
                    bool match = false;
                    bool inverted = (*pat == '!');
                    const char* class = pat + inverted;
                    unsigned char a = *class++;
                    do{
                        unsigned char b = a;

                        if(a=='\0') goto literal;
                        if(class[0] == '-' && class[1] != ']'){
                            b = class[1];
                            if(b == '\0') goto literal;

                            class += 2;
                        }
                        match |= (a <= c && c <= b);
                    }while((a=*class++) != ']');
                    if(match == inverted)
                        goto backtrack;
                    pat = class;
                    break;
            case '\\':
                    d = *pat++;
                    //fallthrough
            default:
literal:
                    if(c == d){
                        if(d == '\0') return true;
                        break;
                    }
backtrack:
                    if(c == '\0' || !back_pat) return false;
                    pat = back_pat;
                    str = ++back_str;
                    break;
        }
    }   
}



char* pattern = "*.c";
int visit(const char* path, const struct stat* stat, int flags, struct FTW* ftw){
    if(glob(pattern, path)){
        printf("file: %s\n", path);
    }
    return 0;
}


int main(){
    if(nftw(".", visit, 10, 0) != 0){
        perror("nftw");
    }
    return 0;
}
