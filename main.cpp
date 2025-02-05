#include "basic.cpp"
#include "basic_math.cpp"
#include "parser.cpp"

int main(void) {
	Arena perm;
	arena_init(&perm);

	double start = get_time_in_seconds();
	Parse_Result parsed = parse(&perm, "../res/test.obj");
	double end = get_time_in_seconds();

	print_format("\n%s\n", parsed.success ? "Success!" : "Error!");
	print_format("Parsed %d line(s) in %.3f ms\n", parsed.lines_parsed, (end - start) * 1000.0);

	return !parsed.success;
}