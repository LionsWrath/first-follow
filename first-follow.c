#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#define MAXLSIZE 1
#define NCHAR 15
#define NRULE 40

#define SETSIZE 100

#define LAMBDA 'e'
#define FINAL '$'

#define START 'S'

//Grammar Related Sets
char **lRules, **rRules;
size_t numRules;

//First Related Sets
char *lfirst, **rfirst;

//Follow Related Sets
char *lfollow, **rfollow;

//-------------------------------------------------------------------READ

void printGrammar() {
    int i;
    printf("Número de Regras: %zu\n", numRules);
    for (i=0; i<numRules; i++) 
        printf("    %s -> %s\n", lRules[i], rRules[i]); 
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

void allocFirst(size_t nlines, size_t nchar) {
    int i;
    lfirst = malloc(nlines * sizeof(char));
    lfirst[0] = '\0';
    rfirst = malloc(nlines * sizeof(char*));
    for (i=0; i<nlines; i++) {
        rfirst[i] = malloc(nchar * sizeof(char));
        rfirst[i][0] = '\0';
    }   
}

void allocFollow(size_t nlines, size_t nchar) {
    int i;
    lfollow = malloc(nlines * sizeof(char));
    lfollow[0] = '\0';
    rfollow = malloc(nlines * sizeof(char*));
    for (i=0; i<nlines; i++) {
        rfollow[i] = malloc(nchar * sizeof(char));
        rfollow[i][0] = '\0';
    }   
}

void readGrammar(char *filename) {
    FILE *input;
    char *line = NULL, *token = NULL;
    size_t len = 0;
    ssize_t read;

    if ((input = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo da gramática!\n");
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

//------------------------------------------------------------------FIRST-FOLLOW

bool isTerminal(char symbol) {
    return !isupper(symbol);
}

void addToSet(char set[], char symbol) {
    int i;

    //Verify if already exists
    for(i=0; set[i] != '\0'; i++)
        if(set[i] == symbol) return;
            
    set[i] = symbol;
    set[i+1] = '\0';
}

bool isEmpty(char set[]) {
    if (strlen(set) == 0) return true;
    return false;
}

void appendSet(char set[], char oth[]) {
    int i;

    if (isEmpty(oth)) return;
    for (i=0; oth[i] != '\0'; i++) 
        addToSet(set, oth[i]);
}

//Like append, but ignores lambda
void concatenateSet(char set[], char oth[]) {
    int i;
    
    if (isEmpty(oth)) return;
    for (i=0; oth[i] != '\0'; i++)
        if (oth[i] != LAMBDA) addToSet(set, oth[i]);
}

bool verifyLambda(char set[]) {
    int i;
    for (i=0; set[i] != '\0'; i++) {
        if (set[i] == LAMBDA) 
            return true;
    }
    return false;
}

int getIdx(char set[], char symbol) {
    int i;
    for (i=0; set[i] != '\0'; i++) {
        if (set[i] == symbol) return i; 
    }

    set[i] = symbol;
    set[i+1] = '\0';

    return i;
}

void printFirstSet() {
    int i, j;

    fprintf(stdout, "\nFirst Sets:\n");
    for (i=0; lfirst[i] != '\0'; i++) {
        fprintf(stdout, "   %c = { ", lfirst[i]);
        for (j=0; j<strlen(rfirst[i])-1; j++) {
            fprintf(stdout, "%c, ", rfirst[i][j]);
        }
        fprintf(stdout, "%c }\n", rfirst[i][strlen(rfirst[i])-1]);
    }
}

void printFollowSet() {
    int i, j;

    fprintf(stdout, "\nFollow Sets:\n");
    for (i=0; lfollow[i] != '\0'; i++) {
        if (!isTerminal(lfollow[i])) {
            fprintf(stdout, "   %c = { ", lfollow[i]);
            for (j=0; j<strlen(rfollow[i])-1; j++) {
                fprintf(stdout, "%c, ", rfollow[i][j]);
            }
            fprintf(stdout, "%c }\n", rfollow[i][strlen(rfollow[i])-1]);
        }
    }
}

//Add all non-terminal symbols to first
void addNTSymbols() {
    int i, j;
    
    for (i=0; i<numRules; i++) 
        for (j=0; j<strlen(rRules[i]); j++) 
            if (isTerminal(rRules[i][j]))
                addToSet(rfirst[getIdx(lfirst, rRules[i][j])], rRules[i][j]);
}

//FIRST
void first(char symbol) {
    int i, j;
    bool foundLambda;

    int idx = getIdx(lfirst, symbol); 
    
    //If X is terminal, FIRST(X) = {X}
    if (isTerminal(symbol)) {
        //addToSet(rfirst[idx], symbol);
        return;
    }

    //If X is non-terminal, read productions
    for (i=0; i<numRules; i++) {
        //X productions
        //Transform in a one vector after
        if (lRules[i][0] == symbol) {
            //Lambda on production
            if (rRules[i][0] == LAMBDA) addToSet(rfirst[idx], LAMBDA);
            else {
                j = 0;
                while (rRules[i][j] != '\0') {
                    foundLambda = false;
                    first(rRules[i][j]);

                    int sidx = getIdx(lfirst, rRules[i][j]);
                    appendSet(rfirst[idx], rfirst[sidx]);
                    foundLambda = verifyLambda(rfirst[sidx]);

                    if (!foundLambda) break;
                    j++;
                }
            }
        } 
    }
}

//FOLLOW
void follow(char symbol) {
    int i, j, idx = getIdx(lfollow, symbol);

    //Add final symbol to start 
    if (symbol == START) {
        addToSet(rfollow[idx], FINAL);
    }
    
    for (i=0; i<numRules; i++) {
        for (j=0; j<strlen(rRules[i]); j++) {
            if (rRules[i][j] == symbol) {
                //A -> sBk - concatenate FIRST(k) to FOLLOW(B)
                if (rRules[i][j+1] != '\0') {
                    int sidx = getIdx(lfirst, rRules[i][j+1]);
                    
                    concatenateSet(rfollow[idx], rfirst[sidx]);

                    //Contatenate FOLLOW(A) to FOLLOW(B)
                    if (verifyLambda(rfirst[sidx]))
                        appendSet(rfollow[idx], rfollow[getIdx(lfollow, lRules[i][0])]);
                }
                //A -> sB - contatenate FOLLOW(A) to FOLLOW(B)
                if (rRules[i][j+1] == '\0') {
                    appendSet(rfollow[idx], rfollow[getIdx(lfollow, lRules[i][0])]);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char filename[100];
    int oc, i;

    while ((oc = getopt(argc, argv, "f:")) != -1) {
        switch (oc) {
            case 'f':
                strcpy(filename, optarg);
                break;
            case '?':
            default:
                fprintf(stderr, "Sem arquivo de entrada.");
                exit(EXIT_FAILURE);
                break;
        }
    }
 
    allocDataset(NRULE, NCHAR);
    readGrammar(filename);
    printGrammar();
    
    allocFirst(SETSIZE, SETSIZE);

    addNTSymbols();
    for (i=0; lRules[i][0] != '\0'; i++) {
        first(lRules[i][0]);
    }
    printFirstSet();

    allocFollow(SETSIZE, SETSIZE);

    int k;
    for (k=0; k<500; k++) {
        for (i=0; lRules[i][0] != '\0'; i++) {
            if (!isTerminal(lRules[i][0])) 
                follow(lRules[i][0]);
        }
    }

    printFollowSet();

    return 0;
}
