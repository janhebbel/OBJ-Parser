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

typedef struct OBJ_Scene OBJ_Scene;
struct OBJ_Scene {
    OBJ_Object *first;
};

typedef struct Parse_Result Parse_Result;
struct Parse_Result {
    OBJ_Scene scene;
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
};

typedef struct Token Token;
struct Token {
    int kind;
    String8 value;
};

enum Token_Kind {
    KIND_NONE,
    KIND_KEYWORD,
    KIND_KEYWORD_BEGIN,
    KIND_KEYWORD_O,
    KIND_KEYWORD_V,
    KIND_KEYWORD_VT,
    KIND_KEYWORD_VN,
    KIND_KEYWORD_F,
    KIND_KEYWORD_END,
    KIND_NAME,
    KIND_FLOAT,
    KIND_INTEGER,
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

bool valid_float(String8 word) {
    int i = 0;
    bool sign = word.start[i] == '-' || word.start[i] == '+';
    i += sign;

    bool dot = word.len > i && word.start[i] == '.';
    bool digit = word.len > i && is_digit(word.start[i]);
    bool next_is_digit = word.len > i + 1 && is_digit(word.start[i + 1]);
    bool valid = digit || (sign && digit) || (dot && next_is_digit) || (sign && dot && next_is_digit);
    i += digit + dot + next_is_digit;

    int decimal_points, e_count;
    decimal_points = 0;
    e_count = 0;
    while (i < word.len && valid && e_count == 0) {
        e_count += ((word.start[i] == 'e') || (word.start[i] == 'E'));
        decimal_points += (word.start[i] == '.');
        if (e_count == 0) {
            valid = valid && (is_digit(word.start[i]) || word.start[i] == '.') && decimal_points <= 1;
        }
        i += 1;
    }

    if (valid && e_count == 1) {
        valid = valid && word.len > i && is_digit(word.start[i]);
        if (!valid) {
            valid = (word.start[i] == '-' || word.start[i] == '+') && word.len > i + 1 && is_digit(word.start[i + 1]);
            i += 1;
        }

        while (i < word.len && valid) {
            e_count += ((word.start[i] == 'e') || (word.start[i] == 'E'));
            valid = valid && e_count == 1 && is_digit(word.start[i]);
            i += 1;
        }
    }

    return valid;
}

bool valid_int(String8 word) {
    int i = 0;
    bool valid, start;
    valid = word.len > 0;
    start = true;
    if (word.len > 0 && (word.start[i] == '+' || word.start[i] == '-')) {
        i += 1;
    }
    while (i < word.len && valid) {
        if (start && word.start[i] == '0') {
            valid = false;
            break;
        }
        if (!is_digit(word.start[i])) {
            valid = false;
            break;
        }
        start = false;
        i += 1;
    }
    return valid;
}

bool valid_primitive_element(String8 word) {
    // int, int/int, int//int, int/int/int
    int i, first_slash, second_slash;
    i = 0, first_slash = -1, second_slash = -1;
    while (i < word.len) {
        if (first_slash == -1 && word.start[i] == '/') {
            first_slash = i;
        } else if (second_slash == -1 && word.start[i] == '/') {
            second_slash = i;
        }
        i += 1;
    }

    bool valid = true;
    if (first_slash == -1) {
        // int
        valid = valid_int(word);
    } else if (first_slash != -1 && second_slash == -1) {
        // int/int
        valid = valid_int({word.start, first_slash}) && 
            valid_int({word.start + first_slash + 1, word.len - first_slash - 1});
    } else if (first_slash != -1 && second_slash != -1 && first_slash + 1 == second_slash) {
        // int//int
        valid = valid_int({word.start, first_slash}) && 
            valid_int({word.start + second_slash + 1, word.len - second_slash - 1});
    } else {
        // int/int/int
        valid = valid && valid_int({word.start, first_slash});
        valid = valid && valid_int({word.start + first_slash + 1, second_slash - first_slash - 1});
        valid = valid && valid_int({word.start + second_slash + 1, word.len - second_slash - 1});
    }

    return valid;
}

bool valid_name(String8 word) {
    bool valid = word.len > 0 && (is_letter(word.start[0]) || word.start[0] == '_');
    for (int i = 1; i < word.len && valid; i += 1) {
        valid = valid && (is_letter(word.start[i]) || is_digit(word.start[i]) || word.start[i] == '_');
    }
    return valid;
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
    if (t->at < t->file.start + t->file.len) {
        Assert(word.len > 0);
        char c = word.start[0];
        if (is_letter(c)) {
            // Keyword or Name
            if (0) {
            } else if (0 == string_compare("o",  word.start, word.len)) {
                token.kind = KIND_KEYWORD_O;
            } else if (0 == string_compare("v",  word.start, word.len)) {
                token.kind = KIND_KEYWORD_V;
            } else if (0 == string_compare("vt", word.start, word.len)) {
                token.kind = KIND_KEYWORD_VT;
            } else if (0 == string_compare("vn", word.start, word.len)) {
                token.kind = KIND_KEYWORD_VN;
            } else if (0 == string_compare("f", word.start, word.len)) {
                token.kind = KIND_KEYWORD_F;
            } else {
                if (valid_name(word)) {
                    token.kind = KIND_NAME;
                } else {
                    printf("%s (%lld): syntax error: Expected a name. Got: %.*s\n", 
                        t->file_name, t->line_number, (int)word.len, word.start);
                    token.kind = KIND_NONE;
                }
            }
        } else if (is_digit(c) || c == '.' || c == '-' || c == '+') {
            // Number or Primitive Element
            bool primitive_element = false;
            bool has_digits = is_digit(c);
            for (int i = 1; i < word.len && has_digits; i += 1) {
                has_digits = is_digit(word.start[i]);
                if (word.start[i] == '/') {
                    primitive_element = true;
                    break;
                }
            }
            if (primitive_element) {
                // Primitive element
                if (valid_primitive_element(word)) {
                    token.kind = KIND_PRIMITIVE_ELEMENT;
                } else {
                    printf("%s (%lld): syntax error: Expected a primitive element. Got: %.*s\n", 
                        t->file_name, t->line_number, (int)word.len, word.start);
                    token.kind = KIND_NONE;
                }
            } else {
                // Number
                if (valid_int(word)) {
                    // Integer
                    token.kind = KIND_INTEGER;
                } else if (valid_float(word)) {
                    // Float
                    token.kind = KIND_FLOAT;
                } else {
                    printf("%s (%lld): syntax error: Expected a number. Got: %.*s\n", 
                        t->file_name, t->line_number, (int)word.len, word.start);
                    token.kind = KIND_NONE;
                }
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

char *token_kind_to_string[] = {
    "none",
    "keyword",
    "keyword begin",
    "o",
    "v",
    "vt",
    "vn",
    "f",
    "keyword end",
    "name",
    "float",
    "integer",
    "primitive element",
    "end of file",
    "count",
};

void print_token(Token t) {
    Arena *scratch = begin_scratch();
    char *cstring = (char*)arena_alloc(scratch, t.value.len + 1);
    snprintf(cstring, t.value.len + 1, "%s", t.value.start);
    printf("[%s, '%s']\n", token_kind_to_string[t.kind], cstring);
    end_scratch();
}

Parse_Result parse(Arena *arena, char *file_name) {
    File file = read_file(arena, file_name);
    if (!file.success) {
        printf("Failed to read file %s.\n", file_name);
    }

    OBJ_Scene scene = {};

    Tokenizer tokenizer = make_tokenizer(file_name, (char *)file.data, file.len);

    struct {
        int low;
        int high;
        int kind;
        int count = 0;
    } expect;
    expect.kind = KIND_KEYWORD;

    bool error = false;
    int curr_keyword = KIND_KEYWORD;
    Token tok = next_token(&tokenizer);
    while (tok.kind != KIND_END_OF_FILE && !error) {
        bool next = true;
        if (expect.kind == KIND_KEYWORD && (KIND_KEYWORD_BEGIN < tok.kind && tok.kind < KIND_KEYWORD_END)) {
            switch (tok.kind) {
            case KIND_KEYWORD_O:
                expect = {1, 1, KIND_NAME};
                break;
            case KIND_KEYWORD_V:
                expect = {3, 4, KIND_FLOAT};
                break;
            case KIND_KEYWORD_VT:
                expect = {2, 3, KIND_FLOAT};
                break;
            case KIND_KEYWORD_VN:
                expect = {3, 3, KIND_FLOAT};
                break;
            case KIND_KEYWORD_F:
                expect = {3, 3, KIND_PRIMITIVE_ELEMENT};
                break;
            default:
                Assert(0 && "This should not happen.");
                error = true;
            }
            curr_keyword = tok.kind;
        } else if (expect.kind != KIND_KEYWORD) {
            if (tok.kind == expect.kind) {
                switch (tok.kind) {
                case KIND_NAME:
                    if (curr_keyword == KIND_KEYWORD_O) {
                        OBJ_Object *obj = (OBJ_Object*)arena_alloc(arena, sizeof(obj));
                        obj->name = tok.value;
                        obj->groups = NULL;
                        scene.first = obj;
                    }
                    break;
                case KIND_FLOAT:
                    break;
                case KIND_INTEGER:
                    break;
                case KIND_PRIMITIVE_ELEMENT:
                    break;
                default:
                    Assert(0 && "This should not happen.");
                    error = true;
                }
                expect.count += 1;
            } else {
                if (expect.low <= expect.count && expect.count <= expect.high) {
                    next = false;
                    expect = {1, 1, KIND_KEYWORD};
                } else {
                    printf("%s (%lld): syntax error: Expected a %s. Got: %s\n",
                        tokenizer.file_name, tokenizer.line_number, 
                        token_kind_to_string[expect.kind], token_kind_to_string[tok.kind]);
                    error = true;
                }
            }
        } else {
            printf("%s (%lld): syntax error: Expected a %s. Got: %s\n",
                tokenizer.file_name, tokenizer.line_number, 
                token_kind_to_string[expect.kind], token_kind_to_string[tok.kind]);
            error = true;
        }

        // print_token(tok);
        if (next) {
            tok = next_token(&tokenizer);
        }
        error = error || tok.kind == KIND_NONE;
    }

    return {scene, tokenizer.line_number, !error && file.success};
}