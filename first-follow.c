#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLSIZE 1
#define NCHAR 15
#define NRULE 40

char **lRules, **rRules;
size_t numRules;

void printGrammar() {
    printf("Number of Rules: %zu\n", numRules);
    for (int i=0; i<numRules; i++) 
        printf("%s -> %s\n", lRules[i], rRules[i]); 
}

void allocDataset(size_t nlines, size_t nchar) {
    lRules = malloc(nlines * sizeof(char*));
    for (int i=0; i<nlines; i++)
        lRules[i] = malloc(MAXLSIZE * sizeof(char));
    
    rRules = malloc(nlines * sizeof(char*));
    for (int i=0; i<nlines; i++)
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

int main() {
    char filename[100];

    if (!scanf("%s", filename)) {
        printf("Falhou na leitura do nome do arquivo!\n");
        exit(EXIT_FAILURE);
    }
 
    allocDataset(NRULE, NCHAR);
    readGrammar(filename);
    printGrammar();

    return 0;
}