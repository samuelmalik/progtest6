#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Sample {
    double frq;
    char* basesString;
}TSAMPLE;

typedef struct SamplesArray {
    int length;
    int capacity;
    TSAMPLE* data;
}TSAMPLES_ARRAY;

int comp(const void *a, const void *b) {
    TSAMPLE* aa = (TSAMPLE*)a;
    TSAMPLE* bb = (TSAMPLE*)b;
    if (aa->frq < bb->frq) return 1;
    if (aa->frq > bb->frq) return -1;
    return 0;
}

void initSamplesArray(TSAMPLES_ARRAY*arr) {
    arr->length = 0;
    arr->capacity = 1;
    arr->data = (TSAMPLE*)malloc(arr->capacity * sizeof(TSAMPLE));
}

void destroySamplesArray(TSAMPLES_ARRAY* arr) {
    for (int i = 0; i < arr->length; i++) {
        free(arr->data[i].basesString);
        arr->data[i].basesString = NULL;
    }
    free(arr->data);
    arr->data = NULL;
    arr->length = 0;
    arr->capacity = 0;
}

void appendSamplesArray(TSAMPLES_ARRAY* arr, TSAMPLE* samples) {
    if (arr->length == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (TSAMPLE*)realloc(arr->data, arr->capacity * sizeof(TSAMPLE));
    }
    arr->data[arr->length++] = *samples;
}

int readSamples(TSAMPLES_ARRAY* arr) {
    printf("Databaze DNA:\n");
    int i=0;
    while (true) {
        // Reading line
        char *line = NULL;
        size_t lineCapacity = 0;
        ssize_t len = getline(&line, &lineCapacity, stdin);

        if (len == EOF) {
            free(line);
            return 1;
        }
        if (len==1&&line[0]=='\n') {
            free(line);
            return 0;
        }
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // string parsing
        char *base;
        double frq = strtod(line, &base);
        // base is not a number or ":" is missing
        if (base == line || base[0]!=':') {
            free(line);
            return 1;
        }
        // get ri of ":"
        base++;

        unsigned long basesLen = strlen(base);
        // check if base is divisible by 3
        if (basesLen%3!=0) {
            free(line);
            return 1;
        }
        // check if base contains only A T G C
        for (long unsigned j=0;j<basesLen;j++) {
            if (base[j]!='A'&&base[j]!='T'&&base[j]!='C'&&base[j]!='G') {
                free(line);
                return 1;
            }
        }

        // saving to samples array
        TSAMPLE newSample;
        newSample.frq = frq;
        newSample.basesString = (char*)malloc(sizeof(char) * len);
        strcpy(newSample.basesString, base);
        appendSamplesArray(arr, &newSample);

        free(line);
        i++;
    }
    return 0;
}
int readSearch(char** searchString) {
    // reading line
    char *line = NULL;
    size_t lineCapacity = 0;
    ssize_t len = getline(&line, &lineCapacity, stdin);
    if (len == EOF) {
        free(line);
        return -1;
    }
    if (len==1&&line[0]=='\n') {
        free(line);
        return 1;
    }
    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // copy read line into output param
    *searchString = (char*)malloc(sizeof(char) * (strlen(line)+1));
    strcpy(*searchString, line);
    free(line);
    return 0;
}
int findSamples(TSAMPLES_ARRAY* arr, char* searchString) {
    // initialize array
    TSAMPLES_ARRAY matchedSamples;
    initSamplesArray(&matchedSamples);

    int found = 0;
    for (int i = 0; i < arr->length;i++) {

        //searching algorithm
        char* foundPtr = arr->data[i].basesString;
        while (*searchString != '\0' && (foundPtr = strstr(foundPtr, searchString))!=NULL) {
            if ((foundPtr-arr->data[i].basesString)%3==0) {
                // if there is a match on index divisible by 3
                found++;

                TSAMPLE matchedSample;
                matchedSample.frq = arr->data[i].frq;
                matchedSample.basesString = (char*)malloc(sizeof(char) * (strlen(arr->data[i].basesString)+1));
                strcpy(matchedSample.basesString, arr->data[i].basesString);
                appendSamplesArray(&matchedSamples, &matchedSample);
                break;
            }
            foundPtr++;
        }
    }
    if (!found)printf("Nalezeno: 0\n");
    else {
        printf("Nalezeno: %d\n", found);
        qsort(matchedSamples.data, matchedSamples.length, sizeof(matchedSamples.data[0]), comp);

        for (int i = 0; i < matchedSamples.length; i++) {
            printf("> %s\n", matchedSamples.data[i].basesString);
        }
    }
    destroySamplesArray(&matchedSamples);
    return 0;
}

int main(void) {
    // init DNA database
    TSAMPLES_ARRAY samplesArr;
    initSamplesArray(&samplesArr);

    // reading samples and storing in dynamic array
    if (readSamples(&samplesArr)) {
        printf("Nespravny vstup.\n");
        destroySamplesArray(&samplesArr);
        return 1;
    }

    // searching
    printf("Hledani:\n");
    while (true) {
        // reading search String
        char *searchString=NULL;
        if (readSearch(&searchString)==-1) {
            free(searchString);
            break;
        }
        long unsigned srcLen = strlen(searchString);
        // check if string is divisible by 3
        if (srcLen%3!=0) {
            printf("Nespravny vstup.\n");
            free(searchString);
            destroySamplesArray(&samplesArr);
            return 1;
        }
        // check if string contains only A T C G
        for (long unsigned j=0;j<srcLen;j++) {
            if (searchString[j]!='A'&&searchString[j]!='T'&&searchString[j]!='C'&&searchString[j]!='G') {
                printf("Nespravny vstup.\n");
                free(searchString);
                destroySamplesArray(&samplesArr);
                return 1;
            }
        }
        // find samples that contains searchString
        findSamples(&samplesArr, searchString);
        free(searchString);
    }
    destroySamplesArray(&samplesArr);
    return 0;
}
