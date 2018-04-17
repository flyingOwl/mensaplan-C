#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "loader.h"
#include "parser.h"
#include "IO.h"


int printTitle(FILE * mealFile){
    FILE * cFile = positionStream(mealFile, SITE_TITLE_START, 1);
    if (!cFile) {
        puts("[FAIL] Format of mealplan could not be recognized!");
        return 1;
    }
    char * cLine = readLine(cFile);
    char * cTemp1 = strchr(cLine,'>');
    char * cTemp2 = strchr(cTemp1,'<');
    if (cTemp1 && cTemp2) {
        cTemp1++;
        cTemp1[cTemp2-cTemp1] = '\0';
        printf("Tagesübersicht: %s\n", cTemp1);
        free(cLine);
    } else {
        return 2;
    }
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
    while(string[len] == ' ' || string[len] == '\n'){
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
    struct mealTopic * head = 0, * next = 0;
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
    if (next) {
        next->nextTopic = malloc(sizeof(struct mealTopic));
        next = next->nextTopic;
        next->nextTopic = 0;
        next->mealList  = 0;
        next->description = "Ende";
        positionStream(cFile, SITE_TABLE_END, 1);
        next->positionInFile = ftell(cFile);
    }
    return head;
}

struct mealTopic * collectMeals(FILE * mealFile, struct mealTopic * cTopics){
    struct mealTopic * nextTopic, * currentTopic = cTopics;
    nextTopic = cTopics->nextTopic;

    while(nextTopic){
        /* printf("Collect between %s and %s\n", currentTopic->description, nextTopic->description);*/
        fseek(mealFile, currentTopic->positionInFile, SEEK_SET);
        struct mealListItem * headItem = 0, * nextItem = 0;
        while(1){
            char * mealLine = seekLine(mealFile, SITE_AMPEL, 0);
            if(ftell(mealFile) >= nextTopic->positionInFile || !mealLine){
                break;
            }
            char * cTemp;
            if(mealLine){
                /* parse meal "color" */
                char * mealColor = strstr(mealLine, SITE_AMPEL);
                mealColor += 7;
                cTemp = strchr(mealColor, '_');
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
                mealFile = positionStream(mealFile, SITE_TOPIC_LINE_FULL, 0);
                char * mealDescription = readLine(mealFile);
                mealDescription = strchr(mealDescription, '>');
                cTemp = strchr(mealDescription, '<');

                if (mealDescription && cTemp) {
                    mealDescription++;
                    *cTemp = '\0';
                    trimTrailingSpace(mealDescription);
                    nextItem->description = strdup(mealDescription);
                }

                /* parse prices */
                char * pLine = seekLine(mealFile, SITE_PRICES_NEXT_LINE, 0);
                char * mealPrices = strstr(nextLine(mealFile), SITE_PRICES_LINE);
                nextItem->priceStudent = calloc(MAX_MALLOC_PRICES_STRING, 1);
                nextItem->priceWorker  = calloc(MAX_MALLOC_PRICES_STRING, 1);
                nextItem->priceForeigner = calloc(MAX_MALLOC_PRICES_STRING, 1);
                int scanfReturn = 0;
                if (mealPrices) {
                    scanfReturn = sscanf(mealPrices, SITE_PRICES_PARSING, nextItem->priceStudent, nextItem->priceWorker, nextItem->priceForeigner);
                }
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

int parsePlan(char * mensaId, int pNextDay, int pColored, int pPrices[3]){
    char * postData = calloc(PLAN_MENSA_ID_STRLEN + 1 + PLAN_DATE_STRLEN + 1, sizeof(char));
    strcat(postData, PLAN_MENSA_ID);
    strcat(postData, "=");
    trimTrailingSpace(mensaId);
    strcat(postData, mensaId);
    if (pNextDay) {
        strcat(postData, "&");
        strcat(postData, PLAN_DATE);
        strcat(postData, getValueNextDay(pNextDay));
    }
    FILE * mealFile = tmpfile();
    if(!mealFile){ return -1; }

    if(downloadPage(PLAN_URL, postData, mealFile)){ free(postData); return -2; }

    puts("");
    struct mealTopic * cTopics = NULL;
    if (!printTitle(mealFile) && (cTopics = collectTopics(mealFile))) {
        cTopics = collectMeals(mealFile, cTopics);
        puts("");
        printMealPlan(cTopics, pColored, pPrices);
    } else {
        printf("[INFO] Please visit the website!\n");
        return 1;
    }
    return 0;
}
