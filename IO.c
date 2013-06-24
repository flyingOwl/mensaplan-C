#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * readLine(FILE * fPointer){
    int charP = 200;
    char * buffer = calloc(200, sizeof(char));
    int charC = 0;

    if(fgets(buffer, charP, fPointer)){
        while(buffer[charP - 2] && buffer[charP - 2] != '\n'){
        /* while string in buffer is not complete... reallocate and read more */
            charC = charP;
            charP *= 2;
            buffer = realloc(buffer, charP * sizeof(char));
            if(!buffer){ // error in realloc - returns NULL
                return NULL;
            } else {
                buffer[charP - 2] = '\0';
                if(fgets(buffer + charC - 1, charC + 1, fPointer) != (buffer + charC -1)){ //fgets did not succed...
                    free(buffer);
                    return NULL;
                }
            }
        }
    } else {
        return NULL;
    }
    return buffer;
}



FILE * positionStream(FILE * fPointer, char * string, int doRewind){
    char * bLine;
    if(doRewind){
        rewind(fPointer);
    }
    while((bLine = readLine(fPointer))){
        char * occurence = strstr(bLine, string);
        if(occurence){
            fseek(fPointer, (occurence - bLine) - strlen(bLine), SEEK_CUR);
            return fPointer;
        }
        free(bLine);
    }
    return NULL;
}

char * seekLine(FILE * fPointer, char * string, int doRewind){
    char * bLine;
    if(doRewind){
        rewind(fPointer);
    }
    while((bLine = readLine(fPointer))){
        char * occurence = strstr(bLine, string);
        if(occurence){
            return bLine;
        }
        free(bLine);
    }
    return NULL;
}
