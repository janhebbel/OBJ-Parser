#include "basic.cpp"
#include "basic_math.cpp"
#include "parser.cpp"

int main(void) {
    S64 global_scratch_len = Kilobytes(16);
    Byte *global_scratch_buf = memory_alloc(global_scratch_len);
    global_scratch_arena = arena_create(global_scratch_buf, global_scratch_len);

    S64 perm_len = Megabytes(2);
    Byte *perm_buf = memory_alloc(perm_len);
    Arena perm = arena_create(perm_buf, perm_len);
    if (!global_scratch_buf || !perm_buf) {
        print_format("Error", "Fatal: Failed to allocate the necessary memory to run this application.");
        return 1;
    }

    char *file_name = "../res/cube.obj";
    double start = get_time_in_seconds();
    File file = read_file(&perm, file_name);
    double mid = get_time_in_seconds();
    Parse_Result parsed = parse(&perm, (char*)file.data, file.len);
    double end = get_time_in_seconds();
    Assert(parsed.success);

    print_format("\n");
    print_format("Read file '%s' in %.3f ms\n", file_name, (mid - start) * 1000.0);
    print_format("Parsed %d lines in %.3f ms\n", parsed.lines_parsed, (end - mid) * 1000.0);
    print_format("_____________________________________________\n");
    print_format("Overall time: %.3f ms\n", (end - start) * 1000.0);

    return !parsed.success;
}