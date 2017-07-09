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
    char * mensaId;
};

enum parseState { IDLE, HASID };

struct MensaList * collectMensen(FILE * htmlFile){
    struct MensaList * head = 0, * next = 0;
    positionStream(htmlFile, MENSA_LIST_START, 1);
    free(readLine(htmlFile));
    int divcount = 1;
    enum parseState _ps = IDLE;
    char * _tempId = NULL;
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

        if (_ps == IDLE && (cTemp = strstr(cLine, "xhrLoad('"))) {
            cTemp += 9;
            (strchr(cTemp, '\''))[0] = '\0';
            _tempId = strdup(cTemp);
            _ps = HASID;
            continue;
        }

        if (_ps == HASID && (cTemp = strstr(cLine, "class=\"dummy\"><div>"))) {
            cTemp += 19;
            (strchr(cTemp, '<'))[0] = '\0';

            if(!head){
                head = malloc(sizeof(struct MensaList));
                next = head;
            } else {
                next->nextItem = malloc(sizeof(struct MensaList));
                next = next->nextItem;
            }
            next->mensaId = _tempId;
            next->mensaName = strdup(cTemp);
            next->nextItem = 0;

            _ps = IDLE;
            continue;
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
		/* flush input buffer... */
		if(strchr(buf, '\n') == NULL){
			while ((i = getchar()) != '\n' && i != EOF);
		}
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
    char * path = malloc(strlen(pw->pw_dir) + strlen(CONFIG_FILE) + 1);
    strcpy(path, pw->pw_dir);
    strcat(path, CONFIG_FILE);
    return path;
}

int initializer(){
    puts("\n### Welcome to the Studentenwerk Mensa application! ###\n");
    FILE * pListPage = tmpfile();
    if(!downloadPage(LIST_URL, NULL, pListPage)){
        struct MensaList * alleMensen = collectMensen(pListPage);
        if (!alleMensen) {
            return 1;
        }

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
        printf("Selected: \"%s\"\n\n", myMensa->mensaName);

        puts("Writing configuration");
        char * cPath = getConfigPath();
        FILE * confy = fopen(cPath,"w+");
        if(confy){
            fprintf(confy, "%s", myMensa->mensaId);
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
