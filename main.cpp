#include "basic.cpp"
#include "parser.cpp"

int main(void) {
    S64 global_scratch_len = Kilobytes(16);
    Byte *global_scratch_buf = memory_alloc(global_scratch_len);
    global_scratch_arena = arena_create(global_scratch_buf, global_scratch_len);

    S64 parse_arena_len = Megabytes(2);
    Byte *parse_arena_buf = memory_alloc(parse_arena_len);
    Arena parse_arena = arena_create(parse_arena_buf, parse_arena_len);
    if (!global_scratch_buf || !parse_arena_buf) {
        print_format("Error", "Fatal: Failed to allocate the necessary memory to run this application.");
        return 1;
    }

    double start = get_time_in_seconds();
    File file = read_file(&parse_arena, "../res/cube.obj");
    double mid = get_time_in_seconds();
    Parse_Result parsed = parse((char*)file.data, file.len);
    double end = get_time_in_seconds();
    Assert(parsed.success);

    print_format("Read file in %.3f ms\n", (mid - start) * 1000.0);
    print_format("Parsing took %.3f ms\n", (end - mid) * 1000.0);
    print_format("_____________________________________________\n");
    print_format("Overall time: %.3f ms\n", (end - start) * 1000.0);

    return !parsed.success;
}