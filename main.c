#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#define READ_END 0
#define WRITE_END 1

#define MAXNUMLETRAS 100 // numero maximo de letras

//cria o pipe
int criaPipe(char** comando, int n);
//quebra o texto
void quebra(char* texto);

int main() {
    
    char texto3[MAXNUMLETRAS];
    fgets(texto3, MAXNUMLETRAS + 1, stdin);
    quebra(texto3);
    return (0);
}

int criaPipe(char** comando, int n) {
    printf("Fork-Pipe: %s\n", comando[n]);
    return 0;
}

void quebra(char* texto) {
    char *argv[256];
    int argc = 0;
    char * token = strtok(texto, "\n |");
    while (token != NULL) {              
        argv[argc] = token;
        criaPipe(argv, argc);
        argc++;
        token = strtok(NULL, " \n |");
    }
}



