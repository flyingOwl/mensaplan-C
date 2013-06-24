#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

#include "loader.h"

int downloadPage(char * pageURL, FILE * filename){
    CURL * myDownload = curl_easy_init();
    if(myDownload && filename){
        int error = 0, perform = 0;
        error += curl_easy_setopt(myDownload, CURLOPT_URL,       pageURL);
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
