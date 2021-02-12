#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 

#define MAXNUMLETRAS 100 // numero maximo de letras
#define MAXCOMANDOS 20 // numero maximo de comandos

//cria o pipe
int criaPipe(char** comando, int n);
//quebra o texto
void quebra(char* texto);
//menu do shell
void menuInicia();

int main() {
    //menu do shell
    menuInicia();
    //char texto[] = "procA | procB | procC";
    char texto[MAXNUMLETRAS];
    //controle de retorno
    int retorno = 0;
    // while (1) {
    //ler o comando
    scanf("%s",&texto);
    quebra(texto);
    //}
    return (EXIT_SUCCESS);
}

int criaPipe(char** comando, int n) {
    for (int i = 0; i < n; i++) {
        printf("Fork-Pipe: %s\n", comando[i]);
    }
    return 0;
}

void quebra(char* texto) {
    char *argv[256];
    int argc = 0;
    char * token = strtok(texto, "\n |");

    while (token != NULL) {
        argv[argc++] = token;
        token = strtok(NULL, " \n |");
    }
    criaPipe(argv, argc);
}

void menuInicia() {
    printf("\n\n\n\t****MENU SHELL****");
    printf("\n\n\n\tDIGITE ALGUM COMANDO ");
    printf("\n");
}
