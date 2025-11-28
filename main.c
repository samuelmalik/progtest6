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
    //arr->data[arr->length].basesString = samples->basesString;
}

int readSamples(TSAMPLES_ARRAY* arr) {
    printf("Databaze DNA:\n");
    int i=0;
    while (true) {

        // citanie riadku
        char *line = NULL;
        size_t lineCapacity = 0;
        ssize_t len = getline(&line, &lineCapacity, stdin);
        if (len == EOF) {
            //printf("Chybaju vyhladavania");
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

        //----zapisovanie do pola vzorkov
        TSAMPLE newSample;
        // parsovanie stringu
        char *base;
        double frq = strtod(line, &base);
        // baza nie je cislo alebo chyba dvojbodka
        if (base == line || base[0]!=':') {
            free(line);
            return 1;
        }
        //zmazanie dvojbodky
        base++;
        // kontrola validity bazy TODO
        unsigned long basesLen = strlen(base);
        // kontrola ci je baza delitelna 3
        if (basesLen%3!=0) {
            free(line);
            return 1;
        }
        //kontrola ci obsahuje len A T C G
        for (long unsigned j=0;j<basesLen;j++) {
            if (base[j]!='A'&&base[j]!='T'&&base[j]!='C'&&base[j]!='G') {
                free(line);
                return 1;
            }
        }

        // ukladanie do pola vzorkov
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
    // citanie riadku
    char *line = NULL;
    size_t lineCapacity = 0;
    ssize_t len = getline(&line, &lineCapacity, stdin);
    if (len == EOF) {
        //printf("Chybaju vyhladavania");
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

    // vratit string
    *searchString = (char*)malloc(sizeof(char) * (strlen(line)+1));
    strcpy(*searchString, line);
    free(line);
    return 0;
}
int findSamples(TSAMPLES_ARRAY* arr, char* searchString) {
    // algoritmus, ktory hladanu vzorku porovna s kazdou v databaze a vypise vysledok

    // vytvorenie pola
    TSAMPLES_ARRAY matchedSamples;
    initSamplesArray(&matchedSamples);
    //debug statement
    //printf("\n=======Tento string hladam:========== %s\n", searchString);

    int found = 0;
    // cyklus prechadzajuci databazu samplov a hladajuci match
    for (int i = 0; i < arr->length;i++) {
        // toto sa spravi pre kazdu vzorku v databaze DNA
        //printf("----Hladam vo vzorke: %s\n", arr->data[i].basesString);
        //algoritmus na hladanie
        char* foundPtr = arr->data[i].basesString;
        while (*searchString != '\0' && (foundPtr = strstr(foundPtr, searchString))!=NULL) {
            if ((foundPtr-arr->data[i].basesString)%3==0) {
                // tu sa bude vysledok appendovat do pola vysledkov
                found++;

                TSAMPLE matchedSample;
                matchedSample.frq = arr->data[i].frq;
                matchedSample.basesString = (char*)malloc(sizeof(char) * (strlen(arr->data[i].basesString)+1));
                strcpy(matchedSample.basesString, arr->data[i].basesString);
                appendSamplesArray(&matchedSamples, &matchedSample);
                //printf("Nalezeno v %s\n", arr->data[i].basesString);
                break;
            }
            //printf("Nasel si na pozici: %zu\n", foundPtr-arr->data[i].basesString);
            foundPtr++;
        }
        //if (!found)printf("Nenasel jsem\n");

    }
    // tu vypis ak sa nenajde nic
    if (!found)printf("Nalezeno: 0\n");
    else printf("Nalezeno: %d\n", found);
    // tu sortovanie a vypis vysledku pre kazdy sample
    for (int i = 0; i < matchedSamples.length; i++) {
        printf("> %s\n", matchedSamples.data[i].basesString);
    }

    destroySamplesArray(&matchedSamples);
    return 0;
}

int main(void) {
    // inicializovat pole samplov tu a poslat do funkcie
    TSAMPLES_ARRAY samplesArr;
    initSamplesArray(&samplesArr);

    // citanie vzorkov
    if (readSamples(&samplesArr)) {
        printf("Nespravny vstup.\n");
        destroySamplesArray(&samplesArr);
        return 1;
    }

    // testovaci vypis vzorkov
    /*
    printf("\nVYPIS VZORKOV:\n");
    for (int j=0; j<samplesArr.length; j++) {
        printf("Frekv: %.2f ", samplesArr.data[j].frq);
        printf("DNA: %s ", samplesArr.data[j].basesString);
        printf("dlzka: %lu\n", strlen(samplesArr.data[j].basesString) );
    }
    */

    // citanie vyhladavania
    printf("Hledani:\n");
    while (true) {
        char *searchString=NULL;
        if (readSearch(&searchString)==-1) {
            free(searchString);
            break;
        }
        long unsigned srcLen = strlen(searchString);
        if (srcLen%3!=0) {
            printf("Nespravny vstup.\n");
            free(searchString);
            destroySamplesArray(&samplesArr);
            return 1;
        }
        //kontrola ci obsahuje len A T C G
        for (long unsigned j=0;j<srcLen;j++) {
            if (searchString[j]!='A'&&searchString[j]!='T'&&searchString[j]!='C'&&searchString[j]!='G') {
                printf("Nespravny vstup.\n");
                free(searchString);
                destroySamplesArray(&samplesArr);
                return 1;
            }
        }
        // funkcia na spracovanie hladani
        findSamples(&samplesArr, searchString);

        free(searchString);
    }
    // uvolnenie pamate
    destroySamplesArray(&samplesArr);
    return 0;

}
