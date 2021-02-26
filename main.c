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
#include <stdbool.h>

#define MAX_NUM_LETRAS 1024 // The maximum length command

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
 * @param *comando[] the comando to forka
 */
int forka(char *comando[]) {
    pid_t pid;
    if (strcmp(comando[0], "sair") != 0) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Falhou");
            return 1;
        } else if (pid == 0) { /* child process */
            execvp(comando[0], comando);
        } else { /* parent process */
            waitpid(pid, NULL, 0);
        }
        ler("/dev/tty");
        salvar("/dev/tty");
    } else {
        return 1;
    }
    return 0;
}

/**
 * Creates a pipe.
 * 
 * @param comando [description]
 */
int criaPipe(char *comando[]) {
    int retorno = 0;
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "Falha na criacao do pipe()");
        return 1;
    }

    dup2(fd[1], 1);
    close(fd[1]);

    retorno = forka(comando);

    dup2(fd[0], 0);
    close(fd[0]);

    return retorno;
}

/*
 * Recebe o texto do usuario e o formata em um vetor de char
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
    char textoUser[MAX_NUM_LETRAS]; // Recebe os parametros do usuario   
    char *token; // recebe o texto do usuario formatado de forma correta
    char *comando[MAX_NUM_LETRAS]; //recebe um comando 
    char *palavra; //recebe uma paralavra do token
    int retorno = 0; //variavel de controle do loop
    int i = 0; //variavel auxiliar para a posicao dos comandos
    while (retorno == 0) {
        printf("$ ");
        //fflush(stdout);    
        fgets(textoUser, MAX_NUM_LETRAS, stdin);

        token = formatar(textoUser);
        palavra = strtok(token, " ");
        i = 0;

        while (palavra) {
            if (*palavra == '|') {
                comando[i] = NULL;
                retorno = criaPipe(comando);
                i = 0;
            } else if (*palavra == '>') {
                salvar(strtok(NULL, " "));
            } else if (*palavra == '<') {
                ler(strtok(NULL, " "));
            } else {
                comando[i] = palavra;
                i++;
            }
            palavra = strtok(NULL, " ");
        }
        if (i != 0) {//caso o usurario digite apenas enter, o programa nao gera exception na hora do fork
            comando[i] = NULL;
            retorno = forka(comando);
        }
    }
    return 0;
}
