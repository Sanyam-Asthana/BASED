#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_CMD_SIZE 64

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
	puts("commands: echo, help, exit");
	return CMD_OK;
}

uint8_t cmd_exit(char** argv, uint8_t argc) {
	return CMD_EXIT;
}

uint8_t cmd_clear(char** argv, uint8_t argc) {
	fputs("\033[2J\033[H", stdout); 
	return CMD_OK;
}

static const command_t cmd_table[] = {
	{ "echo", cmd_echo },
	{ "help", cmd_help },
	{ "exit", cmd_exit },
	{ "clear", cmd_clear },
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

	while(TRUE) {
		fputs("based-0.1$ ", stdout);
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
