#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "loader.h"
#include "parser.h"
#include "IO.h"


int printTitle(FILE * mealFile){
    FILE * cFile = positionStream(mealFile, SITE_CONTENT_START,1);
    char * cLine = readLine(cFile);
    char * cTemp = strchr(cLine,'<');
    cLine[cTemp-cLine] = '\0';
    /* Maybe do some date parsing here, to tell if "today" or other human readable format */
    puts(cLine);
    free(cLine);
    return 0;
}

int getMealColor(char * mColor){
    if(!strcmp(mColor, MEAL_GRUEN)){
        return 1;
    }
    if(!strcmp(mColor, MEAL_ORANGE)){
        return 2;
    }
    if(!strcmp(mColor, MEAL_ROT)){
        return 3;
    }
    return 0;
}

void trimTrailingSpace(char * string){
    int len = strlen(string) - 1;
    while(string[len] == ' '){
        string[len] = '\0';
        len--;
    }
}


struct mealTopic * collectTopics(FILE * mealFile){
    FILE * cFile = positionStream(mealFile, SITE_TABLE_START,0);
    if(!cFile){
        puts("[FAIL] Format of mealplan could not be recognized!");
        return NULL;
    }
    struct mealTopic * head = 0, * next;
    char * cLine, * cTemp;
    while(positionStream(cFile, SITE_TOPIC_LINE, 0)){
        cLine = readLine(cFile);
        if((cLine = strchr(cLine, '>')) && (cTemp = strchr(cLine, '<'))){
            if(head){
                next->nextTopic = malloc(sizeof(struct mealTopic));
                next = next->nextTopic;
            } else {
                head = malloc(sizeof(struct mealTopic));
                next = head;
            }
            next->nextTopic = 0;
            next->mealList  = 0;
            next->positionInFile = ftell(cFile);
            cLine++;
            cTemp[0] = '\0';
            next->description = strdup(cLine);
        }
    }
    return head;
}

struct mealTopic * collectMeals(FILE * mealFile, struct mealTopic * cTopics){
    struct mealTopic * nextTopic, * currentTopic = cTopics;
    nextTopic = cTopics->nextTopic;

    while(nextTopic){
       /*  printf("Collect between %s and %s\n", currentTopic->description, nextTopic->description); */
        fseek(mealFile, currentTopic->positionInFile, SEEK_SET);
        struct mealListItem * headItem = 0, * nextItem = 0;
        while(1){
            char * mealLine = seekLine(mealFile, "#ampel", 0);
            if(ftell(mealFile) >= nextTopic->positionInFile){
                break;
            }
            char * cTemp;
            if(mealLine){
                /* parse meal "color" */
                char * mealColor = strstr(mealLine, "#ampel");
                mealColor += 7;
                cTemp = strchr(mealColor, '"');
                *cTemp = '\0';
                cTemp++;
                if(currentTopic->mealList){
                    nextItem->nextItem = malloc(sizeof(struct mealListItem));
                    nextItem = nextItem->nextItem;
                } else {
                    headItem = malloc(sizeof(struct mealListItem));
                    currentTopic->mealList = headItem;
                    nextItem = headItem;
                }
                nextItem->nextItem = 0;
                nextItem->color = getMealColor(mealColor);

                /* parse Description */
                int  bracket = 1;
                while(bracket || *cTemp == ' ' || *cTemp == '<'){
                    bracket += (*cTemp == '<') ? 1 : 0;
                    bracket -= (*cTemp == '>') ? 1 : 0;
                    cTemp++;
                }
                char * mealDescription = cTemp;
                cTemp = strchr(cTemp, '<');
                *cTemp = '\0';
                cTemp++;
                trimTrailingSpace(mealDescription);
                nextItem->description = strdup(mealDescription);

                /* parse prices */
                char * pLine = seekLine(mealFile, SITE_PRICES_LINE, 0);
                char * mealPrices = strstr(pLine, SITE_PRICES_LINE);
                mealPrices += SITE_PRICES_LENGTH;
                cTemp = strchr(mealPrices, '<');
                *cTemp = '\0';
                nextItem->priceStudent = calloc(MAX_MALLOC_PRICES_STRING, 1);
                nextItem->priceWorker  = calloc(MAX_MALLOC_PRICES_STRING, 1);
                nextItem->priceForeigner = calloc(MAX_MALLOC_PRICES_STRING, 1);
                int scanfReturn = sscanf(mealPrices, "EUR %[0-9.] / %[0-9.] / %[0-9.]", nextItem->priceStudent, nextItem->priceWorker, nextItem->priceForeigner);
                /* printf("%d\n",scanfReturn); */
                switch (scanfReturn){
                    case -1:
                    case 0: {
                        nextItem->priceStudent = "-.--";
                    } /* Fall through */
                    case 1: {
                        nextItem->priceWorker = nextItem->priceStudent;
                    } /* Fall through */
                    case 2: {
                        nextItem->priceForeigner = nextItem->priceWorker;
                    }
                }
                free(pLine);
                free(mealLine);
            }
        }
        currentTopic = nextTopic;
        nextTopic = nextTopic->nextTopic;
    }
    return cTopics;
}

int countMealItems(struct mealListItem * list){
    int i = 0;
    while(list){
        i++;
        list = list->nextItem;
    }
    return i;
}

void copyColorCode(char * destString, int colorCode){
    switch(colorCode){
        case NUM_MEAL_GRUEN: {
            strcpy(destString, "\033[32m");
            return;
        }
        case NUM_MEAL_ORANGE: {
            strcpy(destString, "\033[33m");
            return;
        }
        case NUM_MEAL_ROT: {
            strcpy(destString, "\033[31m");
            return;
        }
    }
}


void printMealPlan(struct mealTopic * cTopics, int pNotColored, int pPrices[3]){
    while(cTopics->nextTopic){
        printf("%s: (%d)\n",cTopics->description, countMealItems(cTopics->mealList));
        struct mealListItem * iTemp = cTopics->mealList;
        char * priceString = malloc(32);
        char * descString;
        while(iTemp){
            if(!pNotColored){
                descString = malloc(strlen(iTemp->description) + 16);
                copyColorCode(descString, iTemp->color);
                strcat(descString, iTemp->description);
                strcat(descString, "\033[m");
            } else {
                descString = iTemp->description;
            }
            /* sprintf(priceString, "€ %4s  %4s  %4s  ", (pPrices[0]) ? iTemp->priceStudent : "", (pPrices[1]) ? iTemp->priceWorker : "",
                     (pPrices[2]) ? iTemp->priceForeigner : ""); */
            strcpy(priceString, "€ ");
            if(pPrices[0]){
                strcat(priceString, iTemp->priceStudent);
                strcat(priceString, "  ");
            }
            if(pPrices[1]){
                strcat(priceString, iTemp->priceWorker);
                strcat(priceString, "  ");
            }
            if(pPrices[2]){
                strcat(priceString, iTemp->priceForeigner);
                strcat(priceString, "  ");
            }
            printf("%s%s\n", priceString, descString);
            iTemp = iTemp->nextItem;
        }
        puts("");
        cTopics = cTopics->nextTopic;
    }

}

int parsePlan(char * mensaURL, int pNextDay, int pColored, int pPrices[3]){
    char * myURL = malloc(strlen(mensaURL) + PLAN_DAY_STRLEN + 1);
    strcpy(myURL, mensaURL);
    if(pNextDay){
        strcat(myURL, PLAN_NEXT_DAY);
    } else {
        strcat(myURL, PLAN_CURRENT_DAY);
    }

    FILE * mealFile = tmpfile();
    if(!mealFile){ return -1; }
    /* printf("Downloading: %s\n", myURL); */
    if(downloadPage(myURL, mealFile)){ free(myURL); return -2; }

    puts("");
    printTitle(mealFile);

    struct mealTopic * cTopics = collectTopics(mealFile);
    
    if(!cTopics){
        printf("[INFO] Please visit the website:\n[INFO] %s\n", mensaURL);
    } else {
        cTopics = collectMeals(mealFile, cTopics);
        puts("");
        printMealPlan(cTopics, pColored, pPrices);
    }
    return 0;
}
