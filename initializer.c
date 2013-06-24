#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#include "loader.h"
#include "initializer.h"
#include "IO.h"

struct MensaList {
    struct MensaList * nextItem;
    char * mensaName;
    char * mensaURL;
};

struct MensaList * collectMensen(FILE * htmlFile){
    struct MensaList * head = 0, * next = 0;
    positionStream(htmlFile, MENSA_LIST_START, 1);
    free(readLine(htmlFile));
    int divcount = 1;
    while(divcount){
        char * cLine = readLine(htmlFile);
        char * cTemp = cLine;
        /* printf("divcount = %d - %s\n",divcount, cLine); */
        while((cTemp = strstr(cTemp,"<div"))){
            divcount++;
            cTemp++;
        }
        cTemp = cLine;
        while((cTemp = strstr(cTemp,"</div"))){
            divcount--;
            cTemp++;
        }

        if(strstr(cLine, "<li>")){ /* Content line */
            char * cNext, * buffer;
            cTemp = strstr(cLine, "href=");
            if(cTemp){
                cTemp += 8;
                cNext = strchr(cTemp, '"');
                cNext -= 10;
                if(!cNext){ return NULL; }
                buffer = malloc((cNext - cTemp) + strlen(LIST_URL_LINK) + 1);
                strcpy(buffer, LIST_URL_LINK);
                strncat(buffer, cTemp, (cNext - cTemp));
                if(!head){
                    head = malloc(sizeof(struct MensaList));
                    next = head;
                } else {
                    next->nextItem = malloc(sizeof(struct MensaList));
                    next = next->nextItem;
                }
                next->mensaURL = buffer;
                cTemp = strchr(cTemp,'>') + 1;
                cNext = strchr(cTemp,'<');
                buffer = malloc((cNext - cTemp) + 1);
                strncpy(buffer, cTemp, (cNext - cTemp));
                buffer[(cNext - cTemp)] = '\0';
                next->mensaName = buffer;
                next->nextItem = 0;
            }
        }
        free(cLine);
    }
    return head;
}

int readUserNumber(int min, int max){
    char buf[4], *p;
    int i = 0;
    while(!i){
        if (fgets(buf, sizeof(buf), stdin) != NULL) {
            i = strtol(buf, &p, 10);
            if (buf[0] != '\n' && (*p == '\n' || *p == '\0') && i >= min && i <= max){
                return i;
            } else {
                printf("Try again: ");
                i = 0;
            }
        }
    }
    return 1;
}

struct MensaList * getMensaListElement(struct MensaList * head, int index){
    while(--index){
        head = head->nextItem;
    }
    return head;
}

char * getConfigPath(){
    struct passwd *pw = getpwuid(getuid());
    char * path = malloc(strlen(pw->pw_dir) + 20);
    strcpy(path, pw->pw_dir);
    strcat(path, CONFIG_FILE);
    return path;
}

int initializer(){
    puts("\n### Welcome to the Studentenwerk Mensa application! ###\n");
    FILE * pListPage = tmpfile();
    if(!downloadPage(LIST_URL, pListPage)){
        struct MensaList * alleMensen = collectMensen(pListPage);
        puts("Select your Mensa:\n");
        struct MensaList * mTemp = alleMensen;
        int i = 1;
        while(mTemp){
            printf(" %2d - %s\n",i,mTemp->mensaName);
            i++;
            mTemp = mTemp->nextItem;
        }

        printf("\nMensa number: ");
        int selection = readUserNumber(1, i - 1);
        struct MensaList * myMensa = getMensaListElement(alleMensen, selection);
        printf("Selected mensa %s\n\n", myMensa->mensaName);

        puts("Writing configuration");
        char * cPath = getConfigPath();
        FILE * confy = fopen(cPath,"w+");
        if(confy){
            fprintf(confy, "%s", myMensa->mensaURL);
            fclose(confy);
            printf("Ok > %s\n", cPath);
        } else {
            puts("FAIL!");
        }
        free(cPath);
    } else {
        puts("Error downloading information...");
        return 1;
    }

    return 0;
}
