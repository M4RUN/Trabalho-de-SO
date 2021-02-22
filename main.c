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
#define MAXCOMANDOS 10 // numero maximo de letras

//cria o pipe
static void pipeline(char ***cmd);
//quebra o texto
void quebraEmComandos(char* texto);
void quebraEmPalavras(char** texto, int qtd);

int main() {

    char texto3[MAXNUMLETRAS];
    printf("$");
    fgets(texto3, MAXNUMLETRAS + 1, stdin);
    char *token;
    token = strtok(texto3, " \n");
    while ((strcmp(token, "sair")) != 0) {
        quebraEmComandos(texto3);
        printf("$");
        fgets(texto3, MAXNUMLETRAS + 1, stdin);
        token = strtok(texto3, " \n");
    }
    return (0);
}

static void pipeline(char ***cmd) {
    int fd[2];
    pid_t pid;
    int fdd = 0;

    while (*cmd != NULL) {
        pipe(fd);
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            dup2(fdd, 0);
            if (*(cmd + 1) != NULL) {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            execvp((*cmd)[0], *cmd);
            exit(1);
        } else {
            wait(NULL);
            close(fd[1]);
            fdd = fd[0];
            cmd++;
        }
    }
}

void quebraEmPalavras(char** texto, int qtdC) {
    char *comandos2[qtdC][MAXNUMLETRAS];
    int qtdP = 0, aux = 0;
    char * token;
    //separando cada palavra de cada comando
    for (int i = 0; i < qtdC; i++) {
        token = strtok(texto[i], " \n");
        comandos2[i][aux] = token;
        while (token != NULL) {
            qtdP++;
            aux++;
            token = strtok(NULL, " \n");
            comandos2[i][aux] = token;
        }
        aux = 0;
    }
    //Printando
    for (int i = 0; i < qtdC; i++) {
        printf("Comando:%d", i);
        printf("\n");
        for (int j = 0; j < qtdP; j++) {
            printf("Palavra %d: ", j);
            printf("%s ", comandos2[i][j]);
        }
        printf("\n");
    }
    pipeline(&comandos2);
}

void quebraEmComandos(char* texto) {
    char *comandos[MAXCOMANDOS];
    int qtd = 0;
    char * token = strtok(texto, "|");
    comandos[qtd] = token;
    //separando os comandos
    while (token != NULL) {
        qtd++;
        token = strtok(NULL, "|");
        comandos[qtd] = token;
    }
    quebraEmPalavras(comandos, qtd);
}





