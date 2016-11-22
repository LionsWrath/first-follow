#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#define NCHAR 15
#define NRULE 40

#define SETSIZE 100

#define LAMBDA 'e'
#define FINAL '$'

#define START 'S'

//Grammar Related Sets
char *lRules, **rRules;
size_t numRules;

//First Related Sets
char *lfirst, **rfirst;

//Follow Related Sets
char *lfollow, **rfollow;

//Table Related Sets
char ***table;
size_t numT, numNT;

//-------------------------------------------------------------------MEMORY
//Create Free functions

void allocDataset(size_t nlines, size_t nchar) {
    int i;
    lRules = malloc(nlines * sizeof(char));
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

void allocTable(size_t nNT, size_t nT) {
    int i, j;
    table = malloc(nNT * sizeof(char*));
    for (i=0; i<nNT; i++) {
        table[i] = malloc(nT * sizeof(char*));
        for (j=0; j<nT; j++) {
            table[i][j] = NULL;
        }
    }
}

//-------------------------------------------------------------------READ

void printGrammar() {
    int i;
    printf("Number of Rules: %zu\n", numRules);
    for (i=0; i<numRules; i++) 
        printf("    %c -> %s\n", lRules[i], rRules[i]); 
}

char* cleanString(char str[], char ch) {
   int i, j = 0, size;
   char *newStr, ch1;

   size = strlen(str);
   newStr = malloc(size * sizeof(char));

   for (i = 0; i < size; i++) {
      if (str[i] != ch) {
         ch1 = str[i];
         newStr[j] = ch1;
         j++;
      }
   }
   newStr[j] = '\0';

   return newStr;
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
        line = cleanString(line, ' ');
        char actual = line[0];
        line = &line[2];
        while ((token = strsep(&line, "|")) != NULL) {
            lRules[numRules] = actual;
          
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

bool verifyFinal(char set[]) {
    int i;
    for (i=0; set[i] != '\0'; i++) {
        if (set[i] == FINAL) 
            return true;
    }
    return false;
}

//Change name to getIndex after
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
        if (lfirst[i] != FINAL) {
            fprintf(stdout, "   %c = { ", lfirst[i]);
            for (j=0; j<strlen(rfirst[i])-1; j++) {
                fprintf(stdout, "%c, ", rfirst[i][j]);
            }
            fprintf(stdout, "%c }\n", rfirst[i][strlen(rfirst[i])-1]);
        }
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

void addFinalSymbol() {
    addToSet(rfirst[getIdx(lfirst, FINAL)], FINAL);
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
        if (lRules[i] == symbol) {
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
                        appendSet(rfollow[idx], rfollow[getIdx(lfollow, lRules[i])]);
                }
                //A -> sB - contatenate FOLLOW(A) to FOLLOW(B)
                if (rRules[i][j+1] == '\0') {
                    appendSet(rfollow[idx], rfollow[getIdx(lfollow, lRules[i])]);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------TABLE

void printTable() {
    int i, j, k;
    int lidx; //position of FINAL

    fprintf(stdout, "\nTable(%zux%zu):\n\n", numNT, numT);
    fprintf(stdout, "  |");
    for (i=0; i<numT; i++) 
        if (lfirst[i] != LAMBDA) fprintf(stdout, "%-10c |", lfirst[i]);
        else lidx = i;
    fprintf(stdout, "\n");
    
    for (i=0; i<numNT; i++) {
        fprintf(stdout, "%c |", lfirst[numT + i]);
        for (j=0; j<numT; j++) {
            if (lidx != j) {
                if (table[i][j] == NULL)
                    fprintf(stdout, "%-10s |", "ERRO");
                else 
                    fprintf(stdout, "%c -> %-5s |", lfirst[numT + i], table[i][j]);
            }
        }
        fprintf(stdout, "\n");
    }
}

void countSymbols() {
    int i;

    numT = numNT = 0;
    for (i=0; i<strlen(lfirst); i++) {
        if (isTerminal(lfirst[i])) numT++;
        else numNT++;
    }
}

int whereis(char set[], char symbol) {
    int i;
    
    for (i=0; i<strlen(set); i++) {
        if (set[i] == symbol) return i;
    }
}

int getTableIndex(char symbol) {
    int gidx = whereis(lfirst, symbol);
    if (isTerminal(symbol)) return gidx; 
    else return gidx - numT;
}

void generateTable() {
    int i, j;

    for (i=0; i<numRules; i++) {
        //Index of first(alpha) in first set
        //fprintf(stdout, "%c -> %s\n", lRules[i], rRules[i]);
        int fidx = getIdx(lfirst, rRules[i][0]);
        for (j=0; j<strlen(rfirst[fidx]); j++) {
            //For each terminal a in FIRST(alpha), include A -> alpha in Table[A,a]
            int Tidx = getTableIndex(rfirst[fidx][j]);
            int NTidx = getTableIndex(lRules[i]);
            table[NTidx][Tidx] = rRules[i];
        }

        //If LAMBDA belongs to FIRST(A)
        if (verifyLambda(rfirst[fidx])) {
            //Index of follow(A) in follow set
            int widx = getIdx(lfollow, lRules[i]);
            for (j=0; j<strlen(rfollow[widx]); j++) {
                int Tidx = getTableIndex(rfollow[widx][j]);
                int NTidx = getTableIndex(lRules[i]);
                table[NTidx][Tidx] = rRules[i];
            }

            //Verify if FINAL belongs to FOLLOW(A)
            if (verifyFinal(rfollow[widx])) {
                int Tidx = getTableIndex(FINAL);
                int NTidx = getTableIndex(lRules[i]);
                table[NTidx][Tidx] = rRules[i];
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
 
    //Load Grammar
    allocDataset(NRULE, NCHAR);
    readGrammar(filename);
    printGrammar();

    //Generate First Set
    allocFirst(SETSIZE, SETSIZE);

    addNTSymbols();
    addFinalSymbol();
    for (i=0; lRules[i] != '\0'; i++) {
        first(lRules[i]);
    }
    printFirstSet();

    //Generate Follow set
    allocFollow(SETSIZE, SETSIZE);

    int k;
    for (k=0; k<1000; k++) { 
        for (i=0; lRules[i] != '\0'; i++) {
            if (!isTerminal(lRules[i])) 
                follow(lRules[i]);
        }
    }

    printFollowSet();

    //Generate Table
    countSymbols();
    allocTable(numNT, numT);

    generateTable();

    printTable();

    return 0;
}
