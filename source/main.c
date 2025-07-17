#include "options.h"
#include "pipeline.h"


#define NOB_IMPLEMENTATION
#define ARENA_IMPLEMENTATION
#define STB_IMPLEMENTATION

#include "../includes/arena.h"
#include "../includes/nob.h"

Arena default_arena = {0};
Arena* main_arena = &default_arena;

int main(int argc, char** argv) {
    nob_minimal_log_level = NOB_NO_LOGS;
    Args* args = parse_args(argc, argv);
    if (args->option == CMD_HELP) {
	print_usage(args);
    } else if (args->option == CMD_BUILD) {
	kudo_build(args);
    }
    arena_free(&default_arena);
    return 0;
}
