
#define MAXNUMLETRAS 100 // numero maximo de letras
#define MAXCOMANDOS 20 // numero maximo de comandos
#define READ_END 0
#define WRITE_END 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

/*
 * 
 */

int main() {
    //menu do shell
    menuInicia();
    //char texto[] = "procA | procB | procC";
    char texto[MAXNUMLETRAS];
    //controle de retorno
    int retorno = 0;
    // while (1) {
    //ler o comando
    scanf("%s%*c", &texto);
    quebra(texto);
    //}
    exit(0);
}

void menuInicia();

int criaPipe(char** comando, int n) {
    //Termina de formatar os comandos e os envia
    //para os pipes
    for (int i = 0; i < n; i++) {
        printf("Fork-Pipe: %s\n", comando[i]);
    }
    return 0;
}

//Recebe os dois comandos do pipe já formatados
//E os processa nos pipes

void pipe(char **cmd1, char **cmd2) {

    pid_t pid;
    int fd[2];

    /* create the pipe */
    if (fork() == 0) //fork do filho A
    {
        close(STDOUT_FILENO); //fecha a saída padrão (monitor), que não mais utilizarei no filho A
        dup(fd[WRITE_END]); //substitui a saída padrão pela minha (filho A) ponta de escrita do pipe
        close(fd[WRITE_END]); //libera a ponta de escrita do pipe, já amarrada na saída padrão pelo dup()

        //OBS.: no filho A será mantida aberta a leitura padrão (teclado), para obter dados do usuário
        close(fd[READ_END]); //fecha a minha (filho A) ponta de leitura do pipe, pois não utilizarei-a


        execvp(cmd1[0], cmd1); // substitui o binário do filho A pelo do programa apontado pelo progA
        perror("Falha na substituicao (execvp) do filho A pelo programa ls");
        exit(1); //se o exec der errado, fecha o processo filho A pois não faz sentido continuar
    } // filho A

    // ========== FILHO B ==========
    if (fork() == 0) //fork do filho B
    {
        close(STDIN_FILENO); //fecha a entrada padrão (teclado), que não mais utilizarei no filho B
        dup(fd[READ_END]); //substitui a entrada padrão pela minha (filho B) ponta de leitura do pipe
        close(fd[READ_END]); //libera a ponta de leitura do pipe, já amarrada na entrada padrão pelo dup()

        //OBS.: no filh B será mantida aberta a saída padrão (monitor), para exibir dados ao usuário
        close(fd[WRITE_END]); //fecha a minha (filho B) ponta de escrita do pipe, pois não utilizarei-a

        execvp(cmd2[0], cmd2);
        perror("Falha na substituicao (execvp) do filho B pelo programa tee");
        exit(1); //se o exec der errado, fecha o processo filho B pois não faz sentido continuar
    }
    //Pai fecha as entradas/saídas restantes
    close(fd[WRITE_END]);
    close(fd[READ_END]);

    wait(NULL);
    wait(NULL);
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

