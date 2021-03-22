#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_NUM_LETRAS 100 // O maximo de letras por comando inserido

//funcao de arquivo executada quando ler um char <

void ler(char *localArquivo) {
    int arquivo = open(localArquivo, O_RDONLY);
    dup2(arquivo, 0);
    close(arquivo);
}

//funcao de arquivo executada quando ler um char >

void salvar(char *localArquivo) {
    int arquivo = open(localArquivo, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    dup2(arquivo, 1);
    close(arquivo);

}

//Realiza um fork quando ler um char |

int forka(char *comando[]) {
    pid_t pid;


    if (strcmp(comando[0], "sair") != 0) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Falhou");
            return 1;
        } else if (pid == 0) { //Processo filho
            execvp(comando[0], comando);
        } else { //Processo pai
            waitpid(pid, NULL, 0);
        }
    } else {
        return 1;
    }
    salvar("/dev/tty");
    ler("/dev/tty");
    return 0;
}

//Cria um pipe

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

//Recebe o texto do usuario e o formata em um vetor de char

char * formatar(char *textoUser) {
    int i;
    int j = 0;
    char *formatado = (char *) malloc((MAX_NUM_LETRAS * 2) * sizeof (char));

    // adiciona espacos entre os caracteres especiais
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

    // adcionando null no final
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
        ler("/dev/tty");
        salvar("/dev/tty");
        printf("$ ");
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
                //Verifica se o arquivo existe
                char *filename = strtok(NULL, " ");
                if (access(filename, F_OK) == 0) {
                    ler(filename);
                } else {
                    printf("O arquivo a ser lido não existe!");
                }
            } else {
                comando[i] = palavra;
                i++;
            }
            palavra = strtok(NULL, " ");
        }
        if (i != 0) {//caso o usurario digite apenas enter, o programa nao gera exception
            comando[i] = NULL;
            retorno = forka(comando);
        }
    }
    return 0;
}
