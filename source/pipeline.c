#include "pipeline.h"
#include "globals.h"
#include "lexer.h"
#include "nob.h"

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
}
