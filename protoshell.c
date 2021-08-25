#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define INPUT_SIZE 128 // tamanho máximo de um input do usuario
#define MAX_ARG 10 // número máximo de argumentos
#define INPUT_DEL " \n" // caracteres a serem deletados pelo strtok

// limpando o terminal usando sequências de escape
#define clear() printf("\033[H\033[J")

// declaração antecipada das funções usadas no programa
void iniciar_terminal();
int leitura_input(char* usu_input);
int parse_input_pipe(char* usu_input, char** usu_input_pipe);
int parse_input(char* usu_input, char** parsed_input);
int pwd();
int cd(char** args);
int help();
int exec_prog(char** args);

int main() {
    int flag_controle = 1; // 1 = programa em execução / 0 = exit
    char *usu_input; // input do usuario
    char **usu_input_pipe; // input do usuario passado para ser divido - pipe
    char **parsed_input; // input depois do parse
    char **parsed_input2; // input depois do parse - comando 2
	
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
            free(usu_input_pipe);
            free(parsed_input);
            free(parsed_input2);
            exit(0);
		}

        // verificar se o comando possui pipe
        if (usu_input_pipe[1] != NULL) {
			parse_input(usu_input_pipe[1], parsed_input2);
            // executa o programa que usa pipe
			// exec_prog_piped(parsed_input, parsed_input2);
		}

        // exec comando único - ls, cd, pwd, help, etc...
        else if (parsed_input[0] != NULL) exec_prog(parsed_input);
    }

    return 0;
}

// boas vindas do terminal
void iniciar_terminal() {
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

// pwd - diretorio corrente
int pwd() {
	char current_dir[300];

	getcwd(current_dir, 300);
	printf("\n");
	printf("%s\n", current_dir);
	printf("\n");

	return 1;
}

// cd - mudar diretorio corrente
int cd(char** args) {
	int change_dir_resp;
	
	if (args[1] == NULL) {
		printf("\n### ERRO: sintaxe correta: cd <diretorio>\n\n");
		return -1;
	}
	
	strtok(args[1], "\n");	
	change_dir_resp = chdir(args[1]);
	
	if (change_dir_resp < 0) {
		printf("\n### ERRO: diretorio invalido\n\n");
	}
	else {
		printf("\n### Novo diretorio:");
		pwd();
	}
	
	return 1;
}

int help() {
	printf("\n");
	printf("### Comandos nativos do shell:\n");
	printf(" pwd\n cd\n ls\n cat\n more\n head\n clear\n exit\n");
	return 1;
}

// executa prog caso não haja pipe
int exec_prog(char** args) {
    int p_pid = -1;
	int exec_resp;
	int in, out;

    if (!strcmp(args[0], "cd")) return cd(args);
    if (!strcmp(args[0], "help")) return help();

    p_pid = fork();

    if (p_pid < 0) {
        printf("\n### ERRO: Fork falhou. Abortando...\n\n");
        exit(1);
    } else if (p_pid == 0) {
        
        if (args[1] != NULL && !strcmp(args[1], "<")) {
            in = open(args[2], O_RDONLY);
            if (in < 0) {
                printf("\n### ERRO: arquivo nao encontrado\n\n");
                close(in);
                exit(1);
            }
            dup2(in, 0);
            close(in);
        } 

        if (args[1] != NULL && !strcmp(args[1], ">")) {
            out = open(args[2], O_WRONLY | O_CREAT, 0666);
            if (out < 0) {
                printf("\n### ERRO: nao foi possivel criar o arquivo de saida\n\n");
                close(out);
                exit(1);
            }
            dup2(out, 1);
            close(out);
        } 

        // executa programas
        exec_resp = execvp(args[0], args);
        if (exec_resp < 0 )  {
            printf("\n### ERRO: comando invalido\n\n");
            exit(1);
        }
    } else wait(NULL);


    return 0;
}