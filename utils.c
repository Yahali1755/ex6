#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define CAPACITY_PER_ITERATION 10

int getInt(const char* prompt) {
    int num;
    int character;

    printf("%s", prompt);
    
    // if input of number isnt successful
    if (scanf("%d", &num) != 1) {
        // clear buffer
        while ((character = getchar()) != '\n' && character != EOF);

        return INVALID_INDEX;
    }

    // clear buffer
     while ((character = getchar()) != '\n' && character != EOF);

    return num;
}

char *getString(const char* prompt) {
    int length = 0;
    int character;
    int capacity = CAPACITY_PER_ITERATION;

    char *string = (char *)malloc(capacity * sizeof(char));

    if (string == NULL) 
        return NULL;

    printf("%s", prompt);

    while ((character = getchar()) != '\n' && character != EOF) {
        if (length >= capacity - 1) { 
            capacity *= 2;           
            char *temp = realloc(string, capacity * sizeof(char));

            if (temp == NULL) {
                free(string);

                return NULL;
            }
            string = temp;
        }

        string[length++] = (char)character;
    };

    string[length] = '\0';
    // Decrease to needed size at the end
    char *finalStr = realloc(string, (length + 1) * sizeof(char));
    
    return (finalStr != NULL) ? finalStr : string;
}