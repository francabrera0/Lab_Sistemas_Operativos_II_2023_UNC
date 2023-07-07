#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <curl/curl.h>

int main(int argc, const char *argv[]) {

    omp_set_num_threads(atoi(argv[1]));
    #pragma omp parallel
    {    
        char clientName[32];
        sprintf(clientName, "name=client%d", omp_get_thread_num());

        CURL *curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8537/increment");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, clientName);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);
            CURLcode response = curl_easy_perform(curl);
            if(response != CURLE_OK) {
                perror("Could not perform curl");
            }
            curl_easy_cleanup(curl);
        }
    }
    return 0;   
}