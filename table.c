#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#include "Stack.h"

#define MAXLSIZE 1
#define NCHAR 15
#define NRULE 40

#define SETSIZE 100
#define LAMBDA 'e'
#define FINAL '$'

char **lRules = {"S", "X", "X", "Y", "Y", "Z", "Z"};
char **rRules = {"XYZ", "aXb", "e", "cYZcX", "d", "eZYe", "f"};

char *lfirst = {'S','X','Y','Z'};
char **rfirst = {"acd", "ae", "cd", "ef"};

char *lfollow = {'S','X','Y','Z'};
char **rfollow = {"$", "bcdef", "ef", "cd$"};

size_t numRules = 7;

char *matriz[4][7];

bool isTerminal(char symbol){
    return !isupper(symbol);
}

int where(char* conjunto, char elemento){
    int i;
    for (i = 0; i < strlen(conjunto); i++){
        if (conjunto[i] == elemento)
            return i;
    }
    return -1;
}

// MAIN
int main(int argc, char *argv[]){
    int i, j, k;
    for (i = 0; i < numRules; i++){
        for (j = 0; j < strlen(rRules[i]); j++){
            if (isTerminal(rRules[i][j])){
                // Incluir A-> alpha em M[A, a];
                int m = where(lfirst, rRules[i][j]);
                for (k = 0; k < strlen(rfirst[m]); k++){
                    int n;
                    switch(rfirst[m][k]){
                        case 'a':
                            n = 0;
                            break;
                        case 'b':
                            n = 1;
                            break;
                        case 'c':
                            n = 2;
                            break;
                        case 'd':
                            n = 3;
                            break;
                        case 'e':
                            n = 4;
                            break;
                        case 'f':
                            n = 5;
                            break;
                        case '$':
                            n = 6;
                            break;
                    }
                    matriz[m][n] = rRules[i];
                }

                // Incluir A->alpha para cada b em Follow(A)
                if ( where(rfirst[m], LAMBDA) != -1 ){
                    for (k = 0; k < strlen(rfollow[m]); k++){
                        int n;
                        switch(rfollow[m][k]){
                            case 'a':
                                n = 0;
                                break;
                            case 'b':
                                n = 1;
                                break;
                           case 'c':
                                n = 2;
                                break;
                            case 'd':
                                n = 3;
                                break;
                            case 'e':
                                n = 4;
                                break;
                            case 'f':
                                n = 5;
                                break;
                            case '$':
                                n = 6;
                                break;
                        }
                        matriz[m][n] = rRules[i]; 
                    }
                }

                if ((where(rfirst[m], LAMBDA) != -1)
                        && (where(FINAL, rfollow[m]) != -1)){
                    matriz[m][6] = rRules[i];
                }

                // Incluir A->alpha em M[A, $]
                if ( (where(rfirst[m], LAMBDA) != -1) 
                        && (where(rfollow[m], FINAL) != -1) ){
                    int m = where(lfollow, lRules[i]);
                    for (k = 0; k < strlen(rfollow[m]); k++){
                        int n;
                        switch(rfollow[m][k]){
                            case 'a':
                                n = 0;
                                break;
                            case 'b':
                                n = 1;
                                break;
                            case 'c':
                                n = 2;
                                break;
                            case 'd':
                                n = 3;
                                break;
                            case 'e':
                                n = 4;
                                break;
                            case 'f':
                                n = 5;
                                break;
                            case '$':
                                n = 6;
                                break;
                        }
                        matriz[m][n] = rRules[i];
                    }
                }
            }
        }    
    }


}
