#pragma once

#include <globals.h>
#include <enum.h>
#include <stdbool.h>

typedef struct {
	bool runalso;
	const char* filename;
} BuildCmd;

typedef struct {
	
} RunCmd;

typedef struct {
	CMD_OPTION  option;
	const char* program_name;
	union {
		BuildCmd build;
		RunCmd   run;
	};
} Args;


Args* parse_args(int argc, char** argv);
void  print_usage(Args* args);
