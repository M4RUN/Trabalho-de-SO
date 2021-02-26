#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_NUM_LETRAS 1024 // The maximum length command

int rodar = 1; // flag to determine when to exit program
int esperar = 1; // flag to determine if process should forka in the background

/**
 *funcao para quando receber o <
 */
void ler(char *localArquivo) {
    int arquivo = open(localArquivo, O_RDONLY);
    dup2(arquivo, 0);
    close(arquivo);
}

/**
 * funcao para quando receber o >
 */
void salvar(char *localArquivo) {
    int arquivo = open(localArquivo, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    dup2(arquivo, 1);
    close(arquivo);
}

/**
 * forkas a command.
 * 
 * @param *argumentos[] the argumentos to forka
 */
void forka(char *argumentos[]) {
    pid_t pid;
    if (strcmp(argumentos[0], "sair") != 0) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Failed");
        } else if (pid == 0) { /* child process */
            execvp(argumentos[0], argumentos);
        } else { /* parent process */
            if (esperar) {
                waitpid(pid, NULL, 0);
            } else {
                esperar = 0;
            }
        }
        ler("/dev/tty");
        salvar("/dev/tty");
    } else {
        rodar = 0;
    }
}

/**
 * Creates a pipe.
 * 
 * @param argumentos [description]
 */
void criaPipe(char *argumentos[]) {
    int fd[2];
    pipe(fd);

    dup2(fd[1], 1);
    close(fd[1]);

    printf("argumentos = %s\n", *argumentos);

    forka(argumentos);

    dup2(fd[0], 0);
    close(fd[0]);
}

/*
 * Recebe o texto do usuario e o formata em um vetor 
 * cada palavra fica em um index
 * Possui um tratamento para os simbolos <, >, |
 * retorna um vetor formatado.
 */
char * formatar(char *textoUser) {
    int i;
    int j = 0;
    char *formatado = (char *) malloc((MAX_NUM_LETRAS * 2) * sizeof (char));

    // add spaces around special characters
    for (i = 0; i < strlen(textoUser); i++) {
        if (textoUser[i] != '>' && textoUser[i] != '<' && textoUser[i] != '|') {
            formatado[j++] = textoUser[i];
        } else {
            formatado[j++] = ' ';
            formatado[j++] = textoUser[i];
            formatado[j++] = ' ';
        }
    }
    formatado[j++] = '\0';

    // add null to the end  
    char *fim;
    fim = formatado + strlen(formatado) - 1;
    fim--;
     *(fim + 1) = '\0';
     
     
    return formatado;
}

int main(void) {
    char *argumentos[MAX_NUM_LETRAS]; // Recebe os parametros do usuario

    while (rodar) {
        printf("$ ");
        //fflush(stdout);

        char textoUser[MAX_NUM_LETRAS];
        fgets(textoUser, MAX_NUM_LETRAS, stdin);

        char *token;
        token = formatar(textoUser);

        if (token[strlen(token) - 1] == '&') {
            esperar = 0;
            token[strlen(token) - 1] = '\0';
        }

        char *arg = strtok(token, " ");
        int i = 0;
        while (arg) {
            if (*arg == '<') {
                ler(strtok(NULL, " "));
            } else if (*arg == '>') {
                salvar(strtok(NULL, " "));
            } else if (*arg == '|') {
                argumentos[i] = NULL;
                criaPipe(argumentos);
                i = 0;
            } else {
                argumentos[i] = arg;
                i++;
            }
            arg = strtok(NULL, " ");
        }
        argumentos[i] = NULL;

        forka(argumentos);
    }
    return 0;
}
