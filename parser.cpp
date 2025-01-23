typedef struct Parse_Result Parse_Result;
struct Parse_Result {
    bool success;
};

typedef struct Tokenizer Tokenizer;
struct Tokenizer {
    // File data
    String8 file;

    // Tokenizer Data
    S64 offset;
};

typedef struct Token Token;
struct Token {
    int kind;
    String8 value;
};

enum Token_Kind {
    KIND_NONE,
    KIND_KEYWORD,
    KIND_VARIABLE,
    KIND_LITERAL,
    KIND_PUNCTUATION,
    KIND_END_OF_LINE,
    KIND_END_OF_FILE,
    KIND_COUNT,
};

Tokenizer make_tokenizer(char *file_data, S64 file_len) {
    Tokenizer t;
    t.offset = 0;
    if (file_data) {
        t.file = {file_data, file_len};
    } else {
        t.file = {"", 0};
    }
    return t;
}

Token tokenizer_next_token(Tokenizer *t) {
    int token_kind = KIND_NONE;
    if (t->offset == t->file.len)
        token_kind = KIND_END_OF_FILE;
    // char c = t->file.start[t->offset]; c;
    t->offset += 1;
    return {token_kind, {}};
}

void print_token(Token t) {
    char *enum_to_string[] = {
        "none",
        "keyword",
        "variable",
        "literal",
        "punctuation",
        "end_of_file",
        "count",
    };

    Arena *scratch = begin_scratch();
    char *cstring = (char*)arena_alloc(scratch, t.value.len + 1);
    snprintf(cstring, t.value.len + 1, "%s", t.value.start);
    print_format("[kind: %s, value: '%s']\n", enum_to_string[t.kind], cstring);
    end_scratch();
}

Parse_Result parse(char *file_data, S64 file_len) {
    Tokenizer tokenizer = make_tokenizer(file_data, file_len);

    Token tok;
    while ((tok = tokenizer_next_token(&tokenizer)).kind != KIND_END_OF_FILE) {
        print_token(tok);
    }

    return {true};
}