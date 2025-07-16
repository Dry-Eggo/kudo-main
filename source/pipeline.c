#include "pipeline.h"
#include "globals.h"
#include "lexer.h"
#include "nob.h"
#include <parser.h>
#include <ccodegen.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

const char* build_directory = NULL;

void ensure_build_dir() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
	elog("Failed to get Current Working Directory\n");
	elog("Build Failed\n");
	exit(1);
    }
    StringBuilder* sb; SB_init(&sb);
    sbapp(sb, "%s/.build", cwd);

    struct stat st = {0};
    if (stat(sb->data, &st) == -1)  {
	printf("Kudo: Creating Build Directory\n");
	mkdir(sb->data, 0755);
    }
    build_directory = str_dup(sb->data);
    SB_free(sb);
}

const char* get_filename(const char* path) {
    char* tmp = str_dup(path);
    char* base = basename(tmp);
    char* res = str_dup(base);
    return res;
}

const char* get_stem(const char* path) {
    const char* filename = get_filename(path);
    char* dot = strrchr(filename, '.');
    if (dot) *dot = '\0';
    return filename;
}

void kudo_build(Args *args) {
	const char* input_path = args->build.filename;
	if (!nob_file_exists(input_path)) {
		elog("No Such File or Directory: '%s'\n", input_path);
		exit(1);
	}
	Nob_String_Builder sb = {0};
	nob_read_entire_file(input_path, &sb);
	Lexer* lexer = kudo_lexer_init(input_path, sb);
	kudo_lexer_lex(lexer);
	printf("Tokens Lexed: %ld\n", lexer->tokens->count);
	Parser* parser = kudo_parser_init(lexer->tokens, sb);
	kudo_parser_parse(parser);
	printf("Top Level Items Retrieved: %ld\n", parser->parsed_program->count);
	CCodegenState* cs = kudo_ccodegen_init(parser->parsed_program, args, sb);
	kudo_ccodegen_kickoff(cs);

	ensure_build_dir();

	const char* filename = get_stem(args->build.filename);
	
	StringBuilder* cfilepath,* ofilepath; SB_init(&cfilepath); SB_init(&ofilepath);
	sbapp(cfilepath, "%s/%s.c", build_directory, filename);
	sbapp(ofilepath, "%s/%s.o", build_directory, filename);

	
	FILE* cfile = fopen(cfilepath->data, "w");
	fprintf(cfile, "%s", cs->output->data);
	fclose(cfile);
	
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "gcc", "-c", "-o", ofilepath->data, cfilepath->data);
	nob_cmd_run_sync(cmd); cmd.count = 0;
	nob_cmd_append(&cmd, "gcc", ofilepath->data, "-o", filename);
	nob_cmd_run_sync(cmd); cmd.count = 0;
}
