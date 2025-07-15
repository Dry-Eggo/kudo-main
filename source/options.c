#include <options.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_usage(Args *args) {
	fprintf(stdout, "Usage: %s [command] [option...]", args->program_name);
	fprintf(stdout, "\ncommand:");
	fprintf(stdout, "\n    build <filename>         compile a kudo file\n");
}


char* shift(int* c, char** v) {
	return v[(*c)++];
}

Args* parse_args(int argc, char **argv) {
	Args* args = arena_alloc(main_arena, sizeof(Args));
	args->program_name = arena_strdup(main_arena, argv[0]);
	if (argc == 1) {
		args->option = CMD_HELP;
		return args;
	}
	// skip the program name & command
	int i = 2;
	const char* command = argv[1];
	if (strcmp(command, "build") == 0) {
		args->option = CMD_BUILD;
		if (argc < 3) {
			print_usage(args);
			fprintf(stderr, "Expected a filename\n");
			exit(1);
		}
		args->build.filename = arena_strdup(main_arena, shift(&i, argv));
		printf("Building: %s\n", args->build.filename);
	} else {
		print_usage(args);
		fprintf(stderr, "Not a Command: %s\n", command);
		exit(1);
	}
	return args;
}
