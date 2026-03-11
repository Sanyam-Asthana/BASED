#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MAX_CMD_SIZE 64

#define GREEN "\e[1;32m"
#define RESET "\e[0m"

typedef enum {
	CMD_OK = 0,
	CMD_ERR = 1,
	CMD_EXIT = 2,
} cmd_status_t;

typedef uint8_t (*cmd_fn)(char** argv, uint8_t argc);

typedef struct {
	const char* name;
	cmd_fn handler;
} command_t;

uint8_t cmd_echo(char** argv, uint8_t argc) {
	for(uint8_t i = 1; i < argc; i++) {
		fputs(argv[i], stdout);

		if(i < argc - 1) {
			fputs(" ", stdout);
		}
	}
	putchar('\n');
	return CMD_OK;
}

uint8_t cmd_help(char** argv, uint8_t argc) {
	puts("commands: echo, help, exit, clear, ls, cat, cd");
	return CMD_OK;
}

uint8_t cmd_exit(char** argv, uint8_t argc) {
	return CMD_EXIT;
}

uint8_t cmd_clear(char** argv, uint8_t argc) {
	fputs("\033[2J\033[H", stdout); 
	return CMD_OK;
}

uint8_t cmd_ls(char** argv, uint8_t argc) {
	const char* path = (argc > 1) ? argv[1] : ".";

	DIR *dir = opendir(path);
	if(dir == NULL) {
		fputs(path, stderr);
		fputs(": directory does not exist!\n", stderr);
		return CMD_ERR;
	}

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		puts(entry->d_name);
	}
	
	closedir(dir);
	return CMD_OK;
}

uint8_t cmd_cat(char** argv, uint8_t argc) {
	if(argc < 2) {
		fputs("usage: cat <filename>\n", stderr);
		return CMD_ERR;
	}
	
	const char* path = argv[1];

	FILE* file = fopen(path, "r");
	if(file == NULL) {
		fputs(path, stderr);
		fputs(": file does not exist!\n", stderr);
		return CMD_ERR;
	}
	
	int current_char;
	while((current_char = getc(file)) != EOF) {
		fputc(current_char, stdout);
	}

	fclose(file);
	return CMD_OK;
}

uint8_t cmd_cd(char** argv, uint8_t argc) {
	if(argc < 2) {
		fputs("usage: cd <path>", stderr);
		return CMD_ERR;
	}

	const char* path = argv[1];

	if(chdir(path) != 0) {
		fputs(path, stderr);
		fputs(": path does not exist!", stderr);
		return CMD_ERR;
	}

	return CMD_OK;
}

static const command_t cmd_table[] = {
	{ "echo", cmd_echo },
	{ "help", cmd_help },
	{ "exit", cmd_exit },
	{ "clear", cmd_clear },
	{ "ls", cmd_ls },
	{ "cat", cmd_cat },
	{ "cd", cmd_cd },
	{ NULL, NULL },
};

uint8_t execute_cmd(char* cmd_buf) {
	
	if(cmd_buf[0] == '\0') {
		return CMD_OK;
	}

	char* argv[8];
	uint8_t argc = 0;

	char *token = strtok(cmd_buf, " ");
	while(token != NULL && argc < 8) {
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}
	
	for(int i = 0; cmd_table[i].name != NULL; i++) {
		if(strcmp(cmd_table[i].name, argv[0]) == 0) {
			cmd_status_t status = cmd_table[i].handler(argv, argc);
			return status;
		}
	}

	fputs(argv[0], stdout);
	puts(": unrecognized command!");
	return CMD_ERR;
}

int main() {

	char cmd_buf[MAX_CMD_SIZE];
	
	puts("BASED V0.1\nSanyam Asthana, 2026");
	puts("Basic Automated Shell for Embedded Devices\n");

	char cwd[128];

	while(TRUE) {
		
		getcwd(cwd, sizeof(cwd));
		fputs(GREEN, stdout);
		fputs(cwd, stdout);
		fputs(RESET, stdout);
			
		fputs(" based-0.1$ ", stdout);
		fflush(stdout);

		uint8_t buf_pos = 0;
		while(TRUE) {
			int next_char = getchar();

			if(next_char == '\n' || next_char == '\r') {
				break;
			}
			
			if(buf_pos < MAX_CMD_SIZE - 1) {
				cmd_buf[buf_pos++] = next_char;
			}
		}

		cmd_buf[buf_pos] = '\0';
		cmd_status_t status = execute_cmd(cmd_buf);

		if(status == CMD_EXIT) {
			return 0;
		}
	}
	return 0;
}
