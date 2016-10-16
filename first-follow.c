#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "Stack.h"

#define MAXLSIZE 1
#define NCHAR 15
#define NRULE 40

#define SETSIZE 100

#define LAMBDA e
#define FINAL $

//Grammar Related Sets
char **lRules, **rRules;
size_t numRules;

//First Related Sets
char firstSet[SETSIZE];

//Follow Related Sets
char followSet[SETSIZE];

//-------------------------------------------------------------------READ

void printGrammar() {
    int i;
    printf("Número de Regras: %zu\n", numRules);
    for (i=0; i<numRules; i++) 
        printf("%s -> %s\n", lRules[i], rRules[i]); 
}

void allocDataset(size_t nlines, size_t nchar) {
    int i;
    lRules = malloc(nlines * sizeof(char*));
    for (i=0; i<nlines; i++)
        lRules[i] = malloc(MAXLSIZE * sizeof(char));
    
    rRules = malloc(nlines * sizeof(char*));
    for (i=0; i<nlines; i++)
        rRules[i] = malloc(nchar * sizeof(char));
    
}

void readGrammar(char *filename) {
    FILE *input;
    char *line = NULL, *token = NULL;
    size_t len = 0;
    ssize_t read;

    if ((input = fopen(filename, "r")) == NULL) {
        printf("Não foi possível abrir o arquivo da gramática!\n");
        exit(EXIT_FAILURE);
    } 
    
    while ((read = getline(&line, &len, input)) != -1) {
        char actual = line[0];
        line = &line[2];
        while ((token = strsep(&line, "|")) != NULL) {
            lRules[numRules][0] = actual;
          
            //Remove trailing newline
            size_t idx = strcspn(token, "\n");
            memmove(&token[idx], &token[idx + 1], strlen(token) - idx);
            
            strcpy(rRules[numRules], token);
            
            numRules++;
        }
    }

    free(line);
    fclose(input);
}

//------------------------------------------------------------------FIRST

bool isTerminal(char symbol) {
    return (symbol >= 'a' && symbol <= 'z');
}

char getFirst(char set[]) {
    return set[0];
}

void first() {

}

int main(int argc, char *argv[]) {
    char filename[100];
    int oc;

    while ((oc = getopt(argc, argv, "f:")) != -1) {
        switch (oc) {
            case 'f':
                strcpy(filename, optarg);
                break;
            case '?':
            default:
                printf("Sem arquivo de entrada.");
                exit(EXIT_FAILURE);
                break;
        }
    }
 
    allocDataset(NRULE, NCHAR);
    readGrammar(filename);
    printGrammar();

    return 0;
}
