typedef struct Parse_Result Parse_Result;
struct Parse_Result {
    S64 lines_parsed;
    bool success;
};

typedef struct Tokenizer Tokenizer;
struct Tokenizer {
    // File data
    String8 file;

    // Tokenizer Data
    char *at;
    S64 line_number = 1;
    int last_keyword;
};

typedef struct Token Token;
struct Token {
    int kind;
    String8 value;
};

enum Token_Kind {
    KIND_NONE,
    KIND_KEYWORD,
    KIND_NAME,
    KIND_NUMBER,
    KIND_PRIMITIVE_ELEMENT,
    KIND_END_OF_FILE,
    KIND_COUNT,
};

Tokenizer make_tokenizer(char *file_data, S64 file_len) {
    Tokenizer t = {};
    if (file_data) {
        t.file = {file_data, file_len};
        t.at = file_data;
    } else {
        t.file = {"", 0};
        t.at = t.file.start;
    }
    return t;
}

Token next_token(Tokenizer *t) {
    Token token;

    String8 text_to_go = {t->at, t->file.len - (t->at - t->file.start)};

    String8 word;
    S64 offset_by;

    // Skip comments and whitespaces.
    while (t->at < t->file.start + t->file.len) {
        if (*t->at == '#') {
            // Skip comment
            word = get_next_line(text_to_go);
            t->at += word.len;
        } else if (is_end_of_line(*t->at)) {
            // Skip end of line
            if (*t->at == '\r') {
                if (t->at + 1 < t->file.start + t->file.len && *(t->at + 1) == '\n') {
                    // \r\n
                    t->at += 2;
                } else {
                    // \r
                    t->at += 1;
                }
            } else {
                // \n
                t->at += 1;
            }
            t->line_number += 1;
        } else if (is_spacing(*t->at)) {
            t->at += 1;
        } else {
            break;
        }
        text_to_go = {t->at, t->file.len - (t->at - t->file.start)};
    }

    // Tokenize next word
    word = get_next_word(text_to_go, " \r\n\t\v\f");
    if (is_letter(*t->at)) {
        // Keyword or Name
        if (0 == string_compare("o",  word.start, word.len) ||
            0 == string_compare("v",  word.start, word.len) ||
            0 == string_compare("vt", word.start, word.len) ||
            0 == string_compare("vn", word.start, word.len) ||
            0 == string_compare("f",  word.start, word.len)) {
            // Keyword
            token.kind = KIND_KEYWORD;
        } else {
            // Name
            token.kind = KIND_NAME;
        }
    } else if (is_digit(*t->at) || *t->at == '.' || *t->at == '-' || *t->at == '+') {
        // Number or Primitive Element
        token.kind = KIND_NUMBER;
    } else if (t->at >= t->file.start + t->file.len) {
        Assert(t->at == t->file.start + t->file.len);
        token.kind = KIND_END_OF_FILE;
    } else {
        // Unhandled token kind
        token.kind = KIND_NONE;
    }
    offset_by = word.len;
    token.value = word;
    t->at += offset_by;
    
    return token;
}

void print_token(Token t) {
    char *enum_to_string[] = {
        "none",
        "keyword",
        "name",
        "number",
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
    while ((tok = next_token(&tokenizer)).kind != KIND_END_OF_FILE) {
        print_token(tok);
    }

    return {tokenizer.line_number, true};
}