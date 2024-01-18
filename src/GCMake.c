#include <stdio.h>
#include <ftw.h>

int visit(const char* path, const struct stat* stat, int flags, struct FTW* ftw){
    printf("path=%s\n", path);
    return 0;
}


int main(){
    if(nftw("src", visit, 10, 0) != 0){
        perror("nftw");
    }
    return 0;
}
