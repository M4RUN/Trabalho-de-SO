/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: MRU
 *
 * Created on 26 de Janeiro de 2021, 11:04
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
/*
 * 
 */
//Liga inicio de pipe a um arquivo

void inIO(char *fileName) {
    int in = open(fileName, O_RDONLY);
    dup2(in, 0);
}


//Liga fim de pipe a um arquivo

void outIO(char *fileName) {
    int out = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    dup2(out, 1);
}

//Compartilha pipes
//Percorre o array de comandos num while e os processa por meio de pipes

static void pipeline(char ***cmd) {
    int fd[2];
    pid_t pid;
    int fdd = 0;
    int in = 0, out = 0; //Tokens para verificar se ha redirecionamento
    int i = 0;
    char* filename;
    char* cmds;

    while (*cmd != NULL) {
        in = 0;
        out = 0;
        while (cmd[i] != NULL) {
            if ((strcmp(cmd[i], ">") == 1)) {
                for (int j = 0; j < i; ++i) {
                    strcpy(cmds[j], cmd[]);
                }
                out = 1;
                strcpy(filename, cmd[i + 1]);
                break;
            } else if (strcmp(cmd[i], "<") == 1) {
                for (int j = 0; j < i; ++i) {
                    strcpy(cmds[j], cmd[]);

                }
                strcpy(filename, cmd[i + 1]);
                in = 1;
                break;
            }
            i++;
        }
        pipe(fd);
        if ((pid = fork()) == -1) {
            perror("Erro! Não foi possível fazer fork!");
            exit(1);
        } else if (pid == 0) {
            if (in) {
                inIO(filename);
                execvp((*cmds)[0], *cmds);
                exit(1);
            } else if (out) {
                outIO(filename);
                execvp((*cmds)[0], *cmds);
                exit(1);
            } else {
                dup2(fdd, 0);
                if (*(cmd + 1) != NULL) {
                    dup2(fd[1], 1);
                }
                close(fd[0]);
                execvp((*cmd)[0], *cmd);
                exit(1);
            }

        } else {

            wait(NULL); /* Collect childs */
            if (in) {
                close(in);
            } else if (out) {
                close(out);
            } else {
                close(fd[1]);
            }
            fdd = fd[0];
            cmd++;
        }
    }
}

//Para fazer cd

void cd(char *cmd) {

    if (cmd[1] == NULL) {
        printf("Endereco de diretorio invalido!");
    } else {
        chdir(cmd[1]);
    }
}

//Para quando o usuário entra uma linha sem pipes

void execucaoSemPipes(char** cmd) {
    // Fazendo o fork
    pid_t pid = fork();

    if (pid == -1) {
        perror("Erro! Não foi possível fazer fork!");
        return;
    } else if (pid == 0) {
        if (execvp(cmd[0], cmd) < 0) {
            perror("Erro! Não foi possível executar o comando!");
        }
        exit(0);
    } else {
        // Espera o filho fechar 
        wait(NULL);

        return;
    }
}

int main(int argc, char** argv) {



    return (EXIT_SUCCESS);
}

