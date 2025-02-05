#include "basic.cpp"
#include "basic_math.cpp"
#include "parser.cpp"

int main(void) {
	Arena perm;
	arena_init(&perm, Megabytes(1));

	char *file_name = "../res/test.obj";
	printf("Starting parse of %s.\n", file_name);

	double start = get_time_in_seconds();
	Parse_Result parsed = parse(&perm, file_name);
	double end = get_time_in_seconds();

	printf("\n%s ", parsed.success ? "Success!" : "Error!");
	printf("Parsed %llu line(s) in %.3f ms\n", parsed.lines_parsed, (end - start) * 1000.0);

	return !parsed.success;
}