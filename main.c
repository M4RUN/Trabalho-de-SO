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
#define MAXCOMANDOS 10 // numero maximo de comandos

//cria os pipes e os forks dos comandos
static void pipeline(char *comandos2[MAXCOMANDOS][MAXNUMLETRAS]); 
//quebra o texto em comandos quando encontra | 
void quebraEmComandos(char* texto);
//Quebra um comando em palavras quando acha espaco
void quebraEmPalavras(char** texto, int qtd);

int main() {
    int qtd = 0, qtd2 = 0, aux = 0;
    char *comandos[MAXNUMLETRAS];
    char *comandos2[MAXCOMANDOS][MAXNUMLETRAS];
    char textoUser[MAXNUMLETRAS];
    printf("$");
    fgets(textoUser, MAXNUMLETRAS + 1, stdin);
    char textoAux[MAXNUMLETRAS];
    strcpy(textoAux, textoUser);
    char * token;
    token = strtok(textoAux, " \n");
    while (strcmp(token, "sair") != 0) {
        token = strtok(textoUser, "|");
        comandos[qtd] = token;
        //separando os comandos
        while (token != NULL) {
            qtd++;
            token = strtok(NULL, "|");
            comandos[qtd] = token;
        }
        //separando cada palavra de cada comando
        for (int i = 0; i < qtd; i++) {
            token = strtok(comandos[i], " \n");
            comandos2[i][aux] = token;
            while (token != NULL) {
                qtd2++;
                aux++;
                token = strtok(NULL, " \n");
                comandos2[i][aux] = token;
            }
            aux = 0;
        }
        pipeline(comandos2);
       
        //retoma
        printf("$");
        fgets(textoUser, MAXNUMLETRAS + 1, stdin);
        strcpy(textoAux, textoUser);
        token = strtok(textoAux, " \n");
    }

    return (0);
}

static void pipeline(char *comandos2[MAXCOMANDOS][MAXNUMLETRAS]) {
     //pipe
        int fd[2],x=0,y=0;
        pid_t pid;
        int fdd = 0;
        while (comandos2[x][0] != NULL) {
            pipe(fd);
            if ((pid = fork()) == -1) {
                perror("fork");
                exit(1);
            } else if (pid == 0) {
                dup2(fdd, 0);
                y++;
                if ((comandos2[x][y]) != NULL) {
                    dup2(fd[1], 1);
                }
                close(fd[0]);
                execvp(comandos2[x][0], &comandos2[x][y]);
                exit(1);
            } else {
                wait(NULL);
                close(fd[1]);
                fdd = fd[0];
                x++;
            }
        }
}



