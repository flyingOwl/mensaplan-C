#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "loader.h"

struct string {
    char * str;
    size_t length;
};

void initString(struct string * s) {
    s->length = 0;
    s->str = malloc(s->length + 1);
    s->str[0] = '\0';
}

size_t writeToString(void * ptr, size_t size, size_t nmemb, struct string * s) {
    size_t _newLength = s->length + (size * nmemb);
    s->str = realloc(s->str, _newLength + 1);
    memcpy(s->str + s->length, ptr, size * nmemb);
    s->str[_newLength] = '\0';
    s->length = _newLength;
    /* return ok */
    return size * nmemb;
}

char * getValueNextDay (int daysInFuture) {
    CURL * myDownload = curl_easy_init();
    if(myDownload){
        struct string _s;
        initString(&_s);
        int error = 0, perform = 0;
        error += curl_easy_setopt(myDownload, CURLOPT_URL, PLAN_DAY_NAVIGATION);
        error += curl_easy_setopt(myDownload, CURLOPT_WRITEFUNCTION, writeToString);
        error += curl_easy_setopt(myDownload, CURLOPT_WRITEDATA, &_s);
        perform = curl_easy_perform(myDownload);
        if(perform){
            const char * cError = curl_easy_strerror(perform);
            printf("ERROR: %s\n", cError);
        }
        curl_easy_cleanup(myDownload);

        size_t _maxLen = strlen(PLAN_DAY_NEXTDAY) + 2;
        char * _splDay = calloc(_maxLen, sizeof(char));
        snprintf(_splDay, _maxLen, "%s%d", PLAN_DAY_NEXTDAY, daysInFuture);

        char * _temp = strstr(_s.str, _splDay);
        if (_temp) {
            _temp = strstr(_temp, PLAN_DAY_VALUE);
            _temp += strlen(PLAN_DAY_VALUE);
            (strchr(_temp, '\''))[0] = '\0';
        }
        return _temp;
    } else {
        return NULL;
    }
}

int downloadPage(char * url, char * params, FILE * filename){
    CURL * myDownload = curl_easy_init();
    if(myDownload && filename){
        int error = 0, perform = 0;
        error += curl_easy_setopt(myDownload, CURLOPT_URL, url);
        if (params) {
            error += curl_easy_setopt(myDownload, CURLOPT_POSTFIELDS, params);
        }
        error += curl_easy_setopt(myDownload, CURLOPT_WRITEDATA, filename);
        perform = curl_easy_perform(myDownload);
        if(perform){
            const char * cError = curl_easy_strerror(perform);
            printf("ERROR: %s\n", cError);
        }
        curl_easy_cleanup(myDownload);
        return (error + perform);
    } else {
        return -1;
    }
}
