#include <stdio.h>

#define PROGRAM_SIZE	4096
#define STACK_SIZE	512
#define DATA_SIZE	65535

#define END		0
#define INC_P		1
#define DEC_P		2
#define INC_V		3
#define DEC_V		4
#define OUT		5
#define IN		6
#define JMP_F		7
#define JMP_B		8

#define SUCCESS		0
#define ERR		1
#define ERR_SE		2
#define ERR_SF		3
#define ERR_SNE		4

int compile(FILE* p);
int execute();

unsigned short program[PROGRAM_SIZE];
unsigned short jmp[PROGRAM_SIZE];
unsigned short data[DATA_SIZE];

unsigned short sc;
unsigned short pc;

int main(int argc, char const *argv[]) {
	int result;
	FILE *fp;
	if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Error: usage \"%s filename\"\n", argv[0]);
		return ERR;
	}

	result = compile(fp);
	fclose(fp);

	switch (result) {
		case SUCCESS:
			fprintf(stdout, "Successfully compiled %u input commands\n", pc);
			return execute();
			break;
		case ERR_SE:
			fprintf(stderr, "Error: stack already empty at command %u\n", pc+1);
			break;
		case ERR_SF:
			fprintf(stderr, "Error: stack full at command %u\n", pc+1);
			break;
		case ERR_SNE:
			fprintf(stderr, "Error: stack not empty at the end of the program (%u)\n", pc+1);
			break;
		case ERR:
		default:
			fprintf(stderr, "Error: at %u\n", pc+1);
			break;
	}
	return ERR;
}

int compile(FILE* p) {
	unsigned short jmp_pc;
	char c;
	pc = PROGRAM_SIZE;
	while (--pc) {
		program[pc] = 0;
		jmp[pc] = 0;
	}
	while ((c = getc(p)) != EOF && pc < PROGRAM_SIZE) {
		switch (c) {
			case '>': program[pc] = INC_P; break;
			case '<': program[pc] = DEC_P; break;
			case '+': program[pc] = INC_V; break;
			case '-': program[pc] = DEC_V; break;
			case '.': program[pc] = OUT; break;
			case ',': program[pc] = IN; break;
			case '[':
				program[pc] = JMP_F;
				if (sc == STACK_SIZE) {
					return ERR_SF;
				}
				sc++;
				jmp_pc = pc;
				break;
			case ']':
				program[pc] = JMP_B;
				if (sc == 0) {
					return ERR_SE;
				}
				sc--;
				jmp[jmp_pc] = pc;
				jmp[pc] = jmp_pc;
				break;
			default: pc--; break;
		}
		pc++;
	}
	if (pc == PROGRAM_SIZE) {
		return ERR;
	}
	if (!(sc == 0)) {
		return ERR_SNE;
	}
	program[pc] = END;
	return SUCCESS;
}

int execute() {
	unsigned short ptr = DATA_SIZE;
	pc = 0;
	while (--ptr) {
		data[ptr] = 0;
	}
	while (program[pc] != END) {
		switch (program[pc]) {
			case INC_P: ptr++; break;
			case DEC_P: ptr--; break;
			case INC_V: data[ptr]++; break;
			case DEC_V: data[ptr]--; break;
			case OUT: putchar((char)data[ptr]); break;
			case IN: data[ptr] = (unsigned int)getchar(); break;
			case JMP_F:
				if (!data[ptr]) {
					pc = jmp[pc];
				}
				break;
			case JMP_B:
				if (data[ptr]) {
					pc = jmp[pc];
				}
				break;
			default: return ERR;
		}
		pc++;
	}
	return SUCCESS;
}
