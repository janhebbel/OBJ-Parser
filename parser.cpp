typedef struct OBJ_Object OBJ_Object;
typedef struct OBJ_Group OBJ_Group;
typedef struct OBJ_Vertex OBJ_Vertex;
typedef U32 OBJ_Index;

struct OBJ_Vertex {
    Vec4F32 v;
    Vec2F32 vt;
    Vec3F32 vn;
};

struct OBJ_Group {
    String8 name;
    OBJ_Vertex *vertices;
    OBJ_Index *indices;
};

struct OBJ_Object {
    String8 name;
    OBJ_Group *groups;
};

typedef struct Parse_Result Parse_Result;
struct Parse_Result {
    OBJ_Vertex *vertices;
    OBJ_Index *indices;
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
    KIND_KEYWORD_O,
    KIND_KEYWORD_V,
    KIND_KEYWORD_VT,
    KIND_KEYWORD_VN,
    KIND_KEYWORD_F,
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

// NOTE(jan): it may be preferable for each keyword to be its own kind
// especially for the primitive elements situation
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
        if (0 == string_compare("f", word.start, word.len)) {
            token.kind = KIND_KEYWORD_F;
            t->last_keyword = KIND_KEYWORD_F;
        } else {
            t->last_keyword = 0;
            if (0) {
            } else if (0 == string_compare("o",  word.start, word.len)) {
                token.kind = KIND_KEYWORD_O;
            } else if (0 == string_compare("v",  word.start, word.len)) {
                token.kind = KIND_KEYWORD_V;
            } else if (0 == string_compare("vt", word.start, word.len)) {
                token.kind = KIND_KEYWORD_VT;
            } else if (0 == string_compare("vn", word.start, word.len)) {
                token.kind = KIND_KEYWORD_VN;
            } else {
                token.kind = KIND_NAME;
            }
        }
    } else if (is_digit(*t->at) || *t->at == '.' || *t->at == '-' || *t->at == '+') {
        // Number or Primitive Element
        if (t->last_keyword == KIND_KEYWORD_F)
            token.kind = KIND_PRIMITIVE_ELEMENT;
        else
            token.kind = KIND_NUMBER;
    } else if (t->at >= t->file.start + t->file.len) {
        // End of file
        Assert(t->at == t->file.start + t->file.len);
        token.kind = KIND_END_OF_FILE;
    } else {
        // Unhandled kind of token
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
        "o",
        "v",
        "vt",
        "vn",
        "f",
        "name",
        "number",
        "primitive element",
        "end of file",
        "count",
    };

    Arena *scratch = begin_scratch();
    char *cstring = (char*)arena_alloc(scratch, t.value.len + 1);
    snprintf(cstring, t.value.len + 1, "%s", t.value.start);
    print_format("[%s, '%s']\n", enum_to_string[t.kind], cstring);
    end_scratch();
}

Parse_Result parse(Arena *arena, char *file_data, S64 file_len) {
    Tokenizer tokenizer = make_tokenizer(file_data, file_len);

    Token tok = next_token(&tokenizer);
    while (tok.kind != KIND_END_OF_FILE) {
        if (tok.kind == KIND_KEYWORD_O) {
            tok = next_token(&tokenizer);
            if (tok.kind == KIND_NAME) {
                
            }
        }

        print_token(tok);
        tok = next_token(&tokenizer);
    }

    return {NULL, NULL, tokenizer.line_number, true};
}