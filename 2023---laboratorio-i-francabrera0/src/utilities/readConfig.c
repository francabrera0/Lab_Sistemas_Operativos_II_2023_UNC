#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/utilities/readConfig.h"

/**
 * @brief Return the value related to the key passed as a parameter in the configuration file.
 * 
 * @param key Key to search in the configuration file
 * @return On success, the correspondent value is returned. On Failure NULL is returned
*/
char* getConfig(const char* key) {
    char* value = NULL;
    char line[100];
    
    FILE* file = fopen("configFile.conf", "r");
    if (file == NULL) {
        perror("Could not open configuration file");
        return NULL;
    }
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        char* equalPosition = strchr(line, '=');
        
        if (equalPosition == NULL)
            continue;
       
        char* lineKey = line;
        char* lineValue = equalPosition + 1;
        *equalPosition = '\0';

        if (strcmp(lineKey, key) == 0) {
            value = malloc(strlen(lineValue) + 1);
            strcpy(value, lineValue);
            break;
        }
    }
    fclose(file);
    return value;
}
