#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define MAX_VARS 32
#define MAX_CMD_LEN 64

typedef struct {
	char id[16];
	int val;
} var_t;

static var_t var_tabl[32];
static uint8_t var_count = 0;

void var_set(char* id, int val) {
    for(int i = 0; i < var_count; i++) {
        if(strcmp(var_tabl[i].id, id) == 0) {
            var_tabl[i].val = val;
            return;
        }
    }

    if(var_count < MAX_VARS) {
        strncpy(var_tabl[var_count].id, id, 15);
        var_tabl[var_count].val = val;
        var_count++;
    }
}

int var_get(char* id) {
	for(int i = 0; i < var_count; i++) {
		if(strcmp(var_tabl[i].id, id) == 0) {
			return var_tabl[i].val;
		}
	}

	return 0;
}

int var_resolve(char* tok) {
	if(strcmp(tok, "zero") == 0) return 0;

	if(tok[0] == '-' || (tok[0] >= '0' && tok[0] <= '9')) {
		return atoi(tok);
	}

	return var_get(tok);
}

void print_int(int n) {
    char buf[12];
    uint8_t i = 0;
    if(n < 0) {
        putchar('-');
        n = -n;
    }
    if(n == 0) {
        putchar('0');
        putchar('\n');
        return;
    }
    while(n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while(i > 0) {
        putchar(buf[--i]);
    }
    putchar('\n');
}

void execute_line(char* line) {
	char* op = strtok(line, " ");
	char* dest = strtok(NULL, " ");
	char* src1 = strtok(NULL, " ");
	char* src2 = strtok(NULL, " ");

	if(strcmp(op, "add") == 0) {
		if(strcmp(dest, "zero") != 0) {
			int result = var_resolve(src1) + var_resolve(src2);
			var_set(dest, result);
		}
	}

	if(strcmp(op, "add") == 0) {
		if(strcmp(dest, "zero") != 0) {
			int result = var_resolve(src1) + var_resolve(src2);
			var_set(dest, result);
		}
	}

	else if(strcmp(op, "puts") == 0) {
		puts(dest);
	}

	else if(strcmp(op, "putv") == 0) {
		print_int(var_resolve(dest));
	}

	else {
		fputs(op, stderr);
		fputs(": unrecognized instruction!", stderr);
	}
}

int main2(int argc, char** argv) {
	if(argc < 2) {
		fputs("usage: planck <filename>", stderr);
	}
	
	const char* path = argv[1];

	FILE* file = fopen(path, "r");

	if(file == NULL) {
		fputs(path, stderr);
		fputs(": file does not exist!", stderr);
		return 1;
	}

	char line[MAX_CMD_LEN];
	while((fgets(line, sizeof(line), file)) != NULL) {
		line[strcspn(line, "\n")] = '\0';
		execute_line(line);
	}
	return 0;
}




