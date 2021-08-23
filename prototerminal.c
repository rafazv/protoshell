// processamento de strings ok
// exit ok
// ls, cd, pwd, help
// >, <, |, &
// execução de programas
// cat, more, head ??

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define INPUT_SIZE 128 // tamanho máximo de um input do usuario
#define MAX_ARG 10 // número máximo de argumentos
#define INPUT_DEL " \n" // caracteres a serem deletados pelo strtok

// limpando o terminal usando sequências de escape
#define clear() printf("\033[H\033[J")
  
// boas vindas do terminal
void iniciar_terminal()
{
    clear();
    printf("******************"
        "************************");
    printf("\n\n\n\t****PROTO TERMINAL****");
    printf("\n\n\n\n*******************"
        "***********************");
    printf("\n");
    sleep(1);
    clear();
    printf("Digite 'help' para listar os comandos disponiveis\n");
    printf("\n");
}

// leitura input
int leitura_input(char* usu_input) {
	fgets(usu_input, INPUT_SIZE, stdin);
	return 1;
}

// input: pipe1 | pipe2
// usu_input_pipe[0] = pipe1
// usu_input_pipe[1] = pipe2
int parse_input_pipe(char* usu_input, char** usu_input_pipe) {
	int i = 0;
	char* buf;
	
	usu_input_pipe[1] = NULL;
	
	buf = strtok(usu_input, "|");
	while (buf != NULL) {
		usu_input_pipe[i++] = buf;
		buf = strtok(NULL, "|");
	}
	while (i < MAX_ARG) usu_input_pipe[i++] = NULL;
	if (usu_input_pipe[1] == NULL) return 0;
	return 1;
}

// divide o input e armazena em parsed_input
int parse_input(char* usu_input, char** parsed_input) {
	int i = 0;
	char* buf;
	
	buf = strtok(usu_input, INPUT_DEL);
	while (buf != NULL) {
		parsed_input[i++] = buf;
		buf = strtok(NULL, INPUT_DEL);
	}
	while (i < MAX_ARG) parsed_input[i++] = NULL;
	return 1;
}

// executa prog caso não haja pipe
int exec_prog(char** args) {
    printf("entrei\n");
    return 0;
}

int main() {
    int flag_controle = 1; // 1 = programa em execução / 0 = exit
    char *usu_input; // input do usuario
    char** usu_input_pipe; // input do usuario passado para ser divido - pipe
    char** parsed_input; // input depois do parse
    char** parsed_input2; // input depois do parse - comando 2
	
    usu_input = malloc(INPUT_SIZE * sizeof(char));
	
	usu_input_pipe = malloc(MAX_ARG * sizeof(char*));
	for (int i = 0; i < MAX_ARG; i++) {
		usu_input_pipe[i] = malloc(INPUT_SIZE * sizeof(char));
	}

    parsed_input = malloc(MAX_ARG * sizeof(char*));
	for (int i = 0; i < MAX_ARG; i++) {
		parsed_input[i] = malloc(INPUT_SIZE * sizeof(char));
	}

    parsed_input2 = malloc(MAX_ARG * sizeof(char*));
	for (int i = 0; i < MAX_ARG; i++) {
		parsed_input2[i] = malloc(INPUT_SIZE * sizeof(char));
	}

    iniciar_terminal(); // intro do terminal

    // loop principal de execução
    while(flag_controle) {
        printf("> ");
        leitura_input(usu_input); // leitura input
        parse_input_pipe(usu_input, usu_input_pipe); // parse do input caso pipe
        parse_input(usu_input_pipe[0], parsed_input); // parse do input - tirando "\n" e espaço

        // verificar se exit
        if (parsed_input[0] != NULL && (!strcmp(parsed_input[0], "exit"))) {	
            flag_controle = 0;
            free(usu_input);
            // free nas outras variáveis ??
            exit(0);
		}

        // verificar se o comando possui pipe
        if (usu_input_pipe[1] != NULL) {
			parse_input(usu_input_pipe[1], parsed_input2);
            // executa o programa que usa pipe
			// exec_prog_piped(parsed_input, parsed_input2);
		}

        // exec comando único - ls, cd, pwd, help
        else if (parsed_input[0] != NULL) exec_prog(parsed_input);
    }

    return 0;
}