typedef struct Parse_Result Parse_Result;
struct Parse_Result {
    bool success;
};

typedef struct Tokenizer Tokenizer;
struct Tokenizer {
    // File data
    String8 file;

    // Tokenizer Data
    char *at;
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
    if (file_data) {
        t.file = {file_data, file_len};
        t.at = file_data;
    } else {
        t.file = {"", 0};
        t.at = t.file.start;
    }
    return t;
}

Token tokenizer_next_token(Tokenizer *t) {
    Token token = {};

    if (t->at >= t->file.start + t->file.len) {
        Assert(t->at == t->file.start + t->file.len);
        token.kind = KIND_END_OF_FILE;
    } else {
        S64 separator_count = 0;
        String8 word = get_next_word({t->at, t->file.len - (t->at - t->file.start)}, " /\n\r\t\v\f", &separator_count);

        t->at += word.len + separator_count;

        if (0 == string_compare_1l("o", word.start, word.len)  || 
            0 == string_compare_1l("v", word.start, word.len)  ||
            0 == string_compare_1l("vt", word.start, word.len) ||
            0 == string_compare_1l("vn", word.start, word.len) ||
            0 == string_compare_1l("f", word.start, word.len)) {
            token.kind = KIND_KEYWORD;
            token.value = word;
        }
    }
    
    return token;
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