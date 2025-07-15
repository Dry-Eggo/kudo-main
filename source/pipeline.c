#include "pipeline.h"
#include "globals.h"
#include "nob.h"

void kudo_build(Args *args) {
	const char* inputpath = args->build.filename;
	if (!nob_file_exists(inputpath)) {
		elog("No Such File or Directory: '%s'\n", inputpath);
		exit(1);
	}
	Nob_String_Builder sb = {0};
	nob_read_entire_file(inputpath, &sb);
}
