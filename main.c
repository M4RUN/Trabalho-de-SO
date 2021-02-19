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

/*
 * 
 */

//Compartilha pipes
//Percorre o array de comandos num while e os processa por meio de pipes

static void pipeline(char ***cmd) {
    int fd[2];
    pid_t pid;
    int fdd = 0; /* Backup */

    while (*cmd != NULL) {
        pipe(fd); /* Sharing bidiflow */
        if ((pid = fork()) == -1) {
            perror("Erro! Não foi possível fazer fork!");
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
            wait(NULL); /* Collect childs */
            close(fd[1]);
            fdd = fd[0];
            cmd++;
        }
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

