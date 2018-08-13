#include <stdio.h>
#include <stdlib.h>
//#include <tokens.h>

int main(int argc, char* argv[]){

    if(argc != 2){
        fprintf(stderr, "Use: vcc file.tpp");
        return 1;
    }

    char *filename = argv[1];

    printf("File name: %s!\n", filename);
    return 0;
}
