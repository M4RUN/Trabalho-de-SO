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
    int fdd = 0; 

    while (*cmd != NULL) {
        pipe(fd); //Começa o processo de pipe
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0)  
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

int main(int argc, char** argv) {
    
    

    return (EXIT_SUCCESS);
}

