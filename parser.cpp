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
    char *file_name;

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
    KIND_INT,
    KIND_FLOAT,
    KIND_PRIMITIVE_ELEMENT,
    KIND_END_OF_FILE,
    KIND_COUNT,
};

Tokenizer make_tokenizer(char *file_name, char *file_data, S64 file_len) {
    Tokenizer t = {};
    t.file_name = file_name;
    if (file_data) {
        t.file = {file_data, file_len};
        t.at = file_data;
    } else {
        t.file = {"", 0};
        t.at = t.file.start;
    }
    return t;
}

bool valid_name (String8 word) {
    bool valid = word.len > 0 && (is_letter(word.start[0]) || word.start[0] == '_');
    for (int i = 1; i < word.len && valid; i += 1) {
        valid = valid && (is_letter(word.start[i]) || is_digit(word.start[i]) || word.start[i] == '_');
    }
    return valid;
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
    if (t->at < t->file.start + t->file.len) {
        Assert(word.len > 0);
        char c = word.start[0];
        if (is_letter(c)) {
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
                    // if (!valid_name(word)) {
                    //     print_format("%s (%d): syntax error: Expected a name.\n", t->file_name, t->line_number);
                    // }
                    token.kind = KIND_NAME;
                }
            }
        } else if (is_digit(c) || c == '.' || c == '-' || c == '+') {
            // Number or Primitive Element
            if (t->last_keyword == KIND_KEYWORD_F) {
                token.kind = KIND_PRIMITIVE_ELEMENT;
            } else {
                token.kind = KIND_INT;
            }
        }
    } else {
        // End of file
        Assert(t->at == t->file.start + t->file.len);
        token.kind = KIND_END_OF_FILE;
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
        "int",
        "float",
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

enum Parser_State {
    STATE_INITIAL,
    STATE_NAME,
    STATE_FLOAT,
    STATE_INT,
};

Parse_Result parse(Arena *arena, char *file_name) {
    File file = read_file(arena, file_name);
    if (!file.success) {
        print_format("Failed to read file %s.\n", file_name);
    }

    Tokenizer tokenizer = make_tokenizer(file_name, (char *)file.data, file.len);

    // int state = STATE_INITIAL;

    bool error = false;
    Token tok = next_token(&tokenizer);
    while (tok.kind != KIND_END_OF_FILE) {
        // switch (state) {
        // case STATE_INITIAL:
        //     switch (tok.kind) {
        //     case KIND_KEYWORD_O:
        //         // expect = {1, KIND_NAME};
        //         break;
        //     case KIND_KEYWORD_V:
        //         // expect = {3, KIND_FLOAT};
        //     case KIND_KEYWORD_VT:

        //     case KIND_KEYWORD_VN:
        //     case KIND_KEYWORD_F:
        //         break;
        //     default:
        //         error = true;
        //         print_format("Error while parsing (%d): Exepected a keyword.\n", tokenizer.line_number);
        //         break;
        //     }
        //     break;
        // }

        // if (error) {
        //     break;
        // }

        print_token(tok);
        tok = next_token(&tokenizer);
    }

    return {NULL, NULL, tokenizer.line_number, !error && file.success};
}