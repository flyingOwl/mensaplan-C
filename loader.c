#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    /* get current date and add x number of days */
    time_t _time = time(NULL);
    struct tm _timeStruct = *localtime(&_time);

    if (daysInFuture == AUTO_NEXT_DAY) {
        /* jump to next day, unless it's a weekend */
        do {
            _timeStruct.tm_mday += 1;
            mktime(&_timeStruct);
        } while (_timeStruct.tm_wday == 0 || _timeStruct.tm_wday == 6);
    } else {
        /* jump x days */
        _timeStruct.tm_mday += daysInFuture;
        mktime(&_timeStruct);
    }
    size_t _maxLen = PLAN_DATE_STRLEN;
    char * _splDay = calloc(_maxLen, sizeof(char));
    snprintf(_splDay, _maxLen, "%d-%d-%d", _timeStruct.tm_year + 1900, _timeStruct.tm_mon + 1, _timeStruct.tm_mday);
    return _splDay;
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
