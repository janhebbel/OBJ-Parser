#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// Useful macros
#define ArrayLen(x) (sizeof(x) / sizeof(*x))

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Sign(x)   ((x > 0) - (x < 0))
#define Abs(x)    ((x < 0) ? -(x) : (x))
#define Clamp(x, min, max) (Max(Min(x, max), min))

#define Kilobytes(x) ((x) * 1024LL)
#define Megabytes(x) (Kilobytes(x) * 1024LL)
#define Gigabytes(x) (Megabytes(x) * 1024LL)

#define IsEnabled(x) ((x) != 0)

#define MemoryCopy(ptr, ptr2, len) memcpy(ptr, ptr2, len)
#define MemorySet(ptr, value, len) memset(ptr, value, len)
#define MemoryZero(ptr, len) MemorySet(ptr, 0, len)


// Use this to declare a dynamic array type.
#define DArray_Type(Name, Type) typedef struct DArray_##Name { Type *data; S64 size; S64 cap; } DArray_##Name

// Limits
#define  S8_MIN  (S8)0x80
#define S16_MIN (S16)0x8000
#define S32_MIN (S32)0x80000000
#define S64_MIN (S64)0x8000000000000000LL

#define  S8_MAX  (S8)0x7f
#define S16_MAX (S16)0x7fff
#define S32_MAX (S32)0x7fffffff
#define S64_MAX (S64)0x7fffffffffffffffLL

#define  U8_MAX  (U8)0xff
#define U16_MAX (U16)0xffff
#define U32_MAX (U32)0xffffffff
#define U64_MAX (U64)0xffffffffffffffffULL

// Types
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float  F32;
typedef double F64;

typedef struct String8 String8;
struct String8 {
	char *start = "";
	size_t len;
};

typedef struct Arena Arena;
struct Arena {
	U8 *base;
	size_t size;
	size_t used;
	size_t minimum_block_size;

	// size_t temp_allocation_size;
};

typedef struct File File;
struct File {
	bool success;
	U8 *data = (U8*)"";
	size_t len;
};

U32 get_page_size(void);

void *allocate_memory(size_t size);
void *reserve_memory(size_t size);
void *commit_memory(void *memory, size_t size);

void exit_process(int return_code);
void notification_window(char *title, char *text);

// Useful functions
bool is_end_of_line(char c) {
	return (c == '\n') || (c == '\r');
}

bool is_spacing(char c) {
	return (c == ' ') || (c == '\t') || (c == '\v') || (c == '\f');
}

bool is_whitespace(char c) {
	return is_spacing(c) || is_end_of_line(c);
}

bool is_letter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

bool is_alphanumeric(char c) {
	return is_letter(c) || is_digit(c);
}

bool is_power_of_two(uintptr_t x) {
	return (x & (x-1)) == 0;
}

S64 next_multiple_of(S64 multiple, S64 value) {
	return (value / multiple + 1) * multiple;
}

S64 next_power_of(S64 power, S64 value) {
	return 0;
}

int string_compare(char *a, char *b) {
	while (*a && *b && *a == *b) {
		a += 1;
		b += 1;
	}
	return *a - *b;
}

int string_compare(char *a, char *b, S64 b_length) {
	int result;
	S64 i = 0;
	while (*a && i < b_length && *a == b[i]) {
		a += 1;
		i += 1;
	}
	if (*a == '\0' && i == b_length) {
		result = 0;
	} else if (i == b_length) {
		result = *a;
	} else {
		result = *a - b[i];
	}
	return result;
}

int string_compare(String8 a, String8 b) {
	int result = 0;

	S64 min_length = Min(a.len, b.len);

	for (S64 i = 0; i < min_length; i += 1) {
		if (a.start[i] != b.start[i]) {
			result = (unsigned char)a.start[i] - (unsigned char)b.start[i];
			break;
		}
	}

	if (a.len < b.len) {
		result = -b.start[a.len];
	} else if (a.len > b.len) {
		result = a.start[b.len];
	}

	return result;
}

String8 get_next_word(String8 text, char *separators, size_t *separator_count = NULL) {
	String8 word;
	word.start = text.start;
	word.len = 0;
	for (size_t i = 0; i < text.len; ++i) {
		bool hit = false;
		char c = text.start[i];
		char *s = separators;
		while (*s) {
			if (*s == c) {
				hit = true;
				break;
			}
			s += 1;
		}
		if (hit) {
			word.len = i;
			break;
		} else if (i == text.len - 1) {
			word.len = i + 1;
			break;
		}
	}
	if (separator_count) {
		for (size_t i = word.len; i < text.len; ++i) {
			char c = text.start[i];
			char *s = separators;
			bool hit = false;
			while (*s) {
				if (*s == c) {
					hit = true;
					break;
				}
				s += 1;
			}
			if (!hit) {
				*separator_count = i - word.len;
				break;
			} else if (i == text.len - 1) {
				*separator_count = i - word.len + 1;
				break;
			}
		}
		if (word.len == text.len) {
			*separator_count = 0;
		}
	}
	return word;
}

String8 get_next_word(String8 text, bool (*test)(char)) {
	String8 word;
	word.start = text.start;
	word.len = 0;
	for (size_t i = 0; i < text.len; ++i) {
		if (!test(text.start[i])) {
			word.len = i;
			break;
		}
		if (i == text.len - 1) {
			word.len = i + 1;
			break;
		}
	}
	return word;
}

String8 get_next_line(String8 text) {
	return get_next_word(text, "\r\n");
}

int string_to_int(char *str, int len) {
	// the maximum characters a 32-bit integer can have, is 11
	char null_terminated_str[12] = {0};
	MemoryCopy(null_terminated_str, str, len);
	int i = atoi(null_terminated_str);
	return i;
}

float string_to_float(char *str, int len) {
	// 1 sign character + largest integral part: 39 + 1 decimal point + fractional part: 7 = 48
	char null_terminated_str[49] = {0};
	MemoryCopy(null_terminated_str, str, len);
	float f = (float)atof(null_terminated_str);
	return f;
}

void *align_forward(void *ptr, size_t alignment) {
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t aligned = (addr + (alignment - 1)) & ~(alignment - 1);
	return (void*)aligned;
}

size_t get_aligned_size(size_t size, size_t alignment) {
	return (size + (alignment - 1)) & ~(alignment - 1);
}

// Arena functions
	arena->base = 0;
	arena->size = 0;
	arena->used = 0;
	arena->minimum_block_size = minimum_block_size; // Must be multiple of OS page size.
	// arena->temp_allocation_size = 0;
}

#define DEFAULT_ALIGNMENT (2 * sizeof(void*))

void *arena_alloc(Arena *a, size_t size, size_t alignment = DEFAULT_ALIGNMENT) {
	size = get_aligned_size(size, alignment);

	void *memory;
	if (a->used + size > a->size) {
		if (!a->base) {
			a->base = (U8*)reserve_memory(Gigabytes(2));
			if (!a->base) {
				printf("Fatal: Failed to reserve 2 GiB of virtual memory.\n");
				exit_process(1);
			}
			printf("Reserved 2 GiB of virtual address space.\n");
		}

		// The minimum amount of memory one can allocate with VirtualAlloc is a single page.
		U32 page_size = get_page_size(); // Windows typically uses 4K pages

		size_t block_size;
		if (size > a->minimum_block_size) {
			if (size % page_size == 0)
				block_size = size;
			else
				block_size = next_multiple_of(page_size, size);
		} else {
			block_size = a->minimum_block_size;
		}

		// TODO(Jan): This is not correct, I think. commit_memory returns an address smaller than a->base + a->used.
		void *result = commit_memory(a->base + a->used, block_size);
		if (!result) {
			printf("Fatal: Failed to commit a memory block of size %llu.\n", block_size);
			exit_process(1);
		}

		printf("Comitted %llu MiB of virtual memory.\n", block_size / (1024 * 1024));

		a->size += block_size;
	}

	memory = a->base + a->used;
	a->used += size;
	return memory;
}

void arena_free_all(Arena *a) {
	a->used = 0;
}

// Temp Arena
// TODO(Jan)

// Hashing function which is effective for ASCII strings: djb2
U64 hash_ascii(char *string) {
	U64 hash = 5381;
	while (*string) {
		hash = (hash << 5) + hash + *string;
		string += 1;
	}
	return hash;
}

U64 hash_ascii(char *string, S64 length) {
	U64 hash = 5381;
	for (S64 i = 0; i < length; i += 1) {
		hash = (hash << 5) + hash + (unsigned char)string[i];
	}
	return hash;
}

//
// OS specific functions

#if defined(_WIN32)
#include <windows.h>
#else
#error Other OSs are currently not supported.
#endif

double get_time_in_seconds(void) {
	LARGE_INTEGER c, f;
	QueryPerformanceCounter(&c);
	QueryPerformanceFrequency(&f);
	return (double)c.QuadPart / (double)f.QuadPart;
}

void *allocate_memory(size_t size) {
	return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void *reserve_memory(size_t size) {
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

void *commit_memory(void *memory, size_t size) {
	return VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

File read_file(Arena *arena, char *path_to_file) {
	File file = {};
	HANDLE file_handle = CreateFile(path_to_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER large_int;
		GetFileSizeEx(file_handle, &large_int);
		Assert(large_int.QuadPart < S32_MAX); // TODO(Jan): Handle bigger files than S32_MAX.
		int file_size = (int)large_int.QuadPart;
		file.data = (U8*)arena_alloc(arena, file_size);
		file.success = ReadFile(file_handle, file.data, file_size, (DWORD*)&file.len, NULL);
		if (!file.success) {
			file.data = (U8*)"";
			file.len = 0;
		}
	}
	return file;
}

void notification_window(char *title, char *text) {
	MessageBox(NULL, text, title, MB_ICONEXCLAMATION);
}

void print_format(char *format, ...) {
	va_list args;
	va_start(args, format);

	vprintf(format, args);

	va_end(args);
}

void exit_process(int return_code) {
	ExitProcess(return_code);
}

U32 get_page_size(void) {
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return (U32)system_info.dwPageSize;
}

// void debug_print_format(char *format, ...) {
// 	va_list args;
// 	va_start(args, format);

// 	Arena *scratch = begin_scratch();

// 	int len = vsnprintf(NULL, 0, format, args);
// 	char *out = (char*)arena_alloc(scratch, (len + 1) * sizeof(char));
// 	vsnprintf(out, len + 1, format, args);
// 	OutputDebugString(out);

// 	end_scratch();

// 	va_end(args);
// }
