#include "basic.cpp"
#include "basic_math.cpp"
#include "parser.cpp"

int main(void) {
    S64 scratch_len = Megabytes(64);
    Byte *scratch_buf = memory_alloc(scratch_len);
    global_scratch_arena = arena_create(scratch_buf, scratch_len);

    S64 perm_len = Megabytes(2);
    Byte *perm_buf = memory_alloc(perm_len);
    Arena perm = arena_create(perm_buf, perm_len);
    if (scratch_buf == NULL || perm_buf == NULL) {
        print_format("Error", "Fatal: Failed to allocate the necessary memory to run this application.");
        return 1;
    }

    char *file_name = "../res/test.obj";
    double start = get_time_in_seconds();
    Parse_Result parsed = parse(&perm, file_name);
    double end = get_time_in_seconds();

    print_format("\n%s\n", parsed.success ? "Success!" : "Error!");
    print_format("Parsed %d line(s) in %.3f ms\n", parsed.lines_parsed, (end - start) * 1000.0);

    return !parsed.success;
}