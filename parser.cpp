typedef struct OBJ_Object OBJ_Object;
typedef struct OBJ_Group OBJ_Group;
typedef struct OBJ_Vertex OBJ_Vertex;
typedef U32 OBJ_Index;

struct OBJ_Vertex {
	Vec4F32 v;
	Vec3F32 vt;
	Vec3F32 vn;
};

struct OBJ_Group {
	String8 name;
	OBJ_Vertex *vertices;
	OBJ_Index *indices;

	OBJ_Group *next;
	OBJ_Group *prev;
};

struct OBJ_Object {
	String8 name;
	OBJ_Vertex *vertices;
	OBJ_Index *indices;
	S64 vertices_count;
	S64 indices_count;

	OBJ_Group *groups_first;
	OBJ_Group *groups_last;

	OBJ_Object *next;
	OBJ_Object *prev;
};

typedef struct OBJ_Scene OBJ_Scene;
struct OBJ_Scene {
	OBJ_Object *objects_first;
	OBJ_Object *objects_last;
};

typedef struct Parse_Result Parse_Result;
struct Parse_Result {
	OBJ_Scene *scene;
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
	String8 value;
	int kind;
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

Tokenizer make_tokenizer(char *file_name, char *file_data, size_t file_len) {
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
		if (start && word.start[i] == '0' && word.len != 1) {
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
	S32 i, first_slash, second_slash;
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
		valid = valid_int({word.start, (size_t)first_slash}) && valid_int({word.start + (size_t)first_slash + 1, word.len - (size_t)first_slash - 1});
	} else if (first_slash != -1 && second_slash != -1 && first_slash + 1 == second_slash) {
		// int//int
		valid = valid_int({word.start, (size_t)first_slash}) && valid_int({word.start + (size_t)second_slash + 1, word.len - (size_t)second_slash - 1});
	} else {
		// int/int/int
		valid = valid && valid_int({word.start, (size_t)first_slash});
		valid = valid && valid_int({word.start + (size_t)first_slash + 1, (size_t)second_slash - (size_t)first_slash - 1});
		valid = valid && valid_int({word.start + (size_t)second_slash + 1, word.len - (size_t)second_slash - 1});
	}

	return valid;
}

bool valid_name(String8 word) {
	bool valid = word.len > 0 && (is_letter(word.start[0]) || word.start[0] == '_');
	for (int i = 1; i < word.len && valid; i += 1) {
		valid = valid && (is_letter(word.start[i]) || is_digit(word.start[i]) || word.start[i] == '_' || word.start[i] == '.' || word.start[i] == '-');
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
					printf("%s (%lld): syntax error: Expected a name. Got: %.*s\n", t->file_name, t->line_number, (int)word.len, word.start);
					token.kind = KIND_NONE;
				}
			}
			if (KIND_KEYWORD_BEGIN < token.kind && token.kind < KIND_KEYWORD_END) {
				t->last_keyword = token.kind;
			}
		} else if (is_digit(c) || c == '.' || c == '-' || c == '+') {
			// Number or Primitive Element
			if (t->last_keyword == KIND_KEYWORD_F) {
				// Primitive element
				if (valid_primitive_element(word)) {
					token.kind = KIND_PRIMITIVE_ELEMENT;
				} else {
					printf("%s (%lld): syntax error: Expected a primitive element. Got: %.*s\n", t->file_name, t->line_number, (int)word.len, word.start);
					token.kind = KIND_NONE;
				}
			} else {
				// Number
				// NOTE(Jan): Don't parse keywords that expect float as int
				if (valid_int(word) && t->last_keyword != KIND_KEYWORD_V && t->last_keyword != KIND_KEYWORD_VT && t->last_keyword != KIND_KEYWORD_VN) {
					// Integer
					token.kind = KIND_INTEGER;
				} else if (valid_float(word)) {
					// Float
					token.kind = KIND_FLOAT;
				} else {
					printf("%s (%lld): syntax error: Expected a number. Got: %.*s\n", t->file_name, t->line_number, (int)word.len, word.start);
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

// TODO(Jan): restore functionality
void print_token(Arena *a, Token t) {
	// Arena temp = begin_temp_arena(a);
	// size_t temp_allocation_size = t.value.len + 1;
	// a->used += temp_allocation_size;
	// {
	// 	char *cstring = (char*)arena_alloc(temp, t.value.len + 1);
	// 	snprintf(cstring, t.value.len + 1, "%s", t.value.start);
	// 	printf("[%s, '%s']\n", token_kind_to_string[t.kind], cstring);
	// }
	// a->used -= temp_allocation_size;
}

//
// Linked list helper functions.
// TODO: test linked list and dynamic array performance
OBJ_Scene *make_scene(Arena *arena) {
	OBJ_Scene *scene = (OBJ_Scene*)arena_alloc(arena, sizeof(*scene));
	MemoryZero(scene, sizeof(*scene));
	return scene;
}

OBJ_Object *make_object(Arena *arena) {
	OBJ_Object *object = (OBJ_Object*)arena_alloc(arena, sizeof(*object));
	MemoryZero(object, sizeof(*object));
	return object;
}

void append_object(OBJ_Scene *scene, OBJ_Object *object) {
	Assert((scene->objects_first != NULL && scene->objects_last != NULL) || (scene->objects_first == NULL && scene->objects_last == NULL));
	if (scene->objects_first == NULL && scene->objects_last == NULL) {
		scene->objects_first = object;
		scene->objects_last = object;
	} else if (scene->objects_first != NULL && scene->objects_last != NULL) {
		object->prev = scene->objects_last;
		scene->objects_last->next = object;
		scene->objects_last = object;
	}
}

void append_group(OBJ_Object *object, OBJ_Group *group) {
	Assert((object->groups_first != NULL && object->groups_last != NULL) || (object->groups_first == NULL && object->groups_last == NULL));
	if (object->groups_first == NULL && object->groups_last == NULL) {
		object->groups_first = group;
		object->groups_last = group;
	} else if (object->groups_first != NULL && object->groups_last != NULL) {
		group->prev = object->groups_last;
		object->groups_last->next = group;
		object->groups_last = group;
	}
}

//
// Parsing
Parse_Result parse(Arena *arena, char *file_name) {
	File file = read_file(arena, file_name);
	if (!file.success) {
		printf("Failed to read file %s.\n", file_name);
	}

	OBJ_Scene *scene = make_scene(arena);

	Vec4F32 *positions = (Vec4F32*)arena_alloc(arena, sizeof(*positions) * 1024 * 1024);
	Vec3F32 *tex_coords = (Vec3F32*)arena_alloc(arena, sizeof(*tex_coords) * 1024 * 1024 * 2);
	Vec3F32 *normals = (Vec3F32*)arena_alloc(arena, sizeof(*normals) * 1024 * 1024 * 2);

	// NOTE(Jan): We leave the first element zeroed. Later we calculate the index and use the following lists to access
	// the correct position, texture coordinate, and normals. This is a neat trick to zero the values for vertices where
	// the obj file does not specify texture coordinates or normals.
	// This is also nice because indices in obj files start with 1 anyway, so parsing an index will have the correct
	// index value directly, without having to subtract one from the parsed value.
	// The indices may just be defined as 0 and are valid this way.
	positions[0] = {};
	tex_coords[0] = {};
	normals[0] = {};

	S64 position_index = 1;
	S64 tex_coord_index = 1;
	S64 normal_index = 1;

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
	OBJ_Object *curr_object = NULL;

	Token tok = next_token(&tokenizer);
	while (tok.kind != KIND_END_OF_FILE && !error) {
		bool next = true;
		if (expect.kind == KIND_KEYWORD && (KIND_KEYWORD_BEGIN < tok.kind && tok.kind < KIND_KEYWORD_END)) {
			switch (tok.kind) {
				case KIND_KEYWORD_O: {
					expect = {1, 1, KIND_NAME};
					break;
				}
				case KIND_KEYWORD_V: {
					expect = {3, 4, KIND_FLOAT};
					break;
				}
				case KIND_KEYWORD_VT: {
					expect = {2, 3, KIND_FLOAT};
					break;
				}
				case KIND_KEYWORD_VN: {
					expect = {3, 3, KIND_FLOAT};
					break;
				}
				case KIND_KEYWORD_F: {
					expect = {3, 3, KIND_PRIMITIVE_ELEMENT};
					break;
				}
				default: {
					Assert(0 && "This should not happen.");
					error = true;
				}
			}
			curr_keyword = tok.kind;
		} else if (expect.kind != KIND_KEYWORD) {
			if (tok.kind == expect.kind) {
				switch (tok.kind) {
					case KIND_NAME: {
						if (curr_keyword == KIND_KEYWORD_O) {
							// NOTE(Jan): 02/02/2025
							// Depending on the name we need to select the correct object or group. To do this, hashing the
							// name would be a good idea. However, the amount of objects and groups is relatively small in
							// most obj files. Therefore, linear search and string_compare are likely enough. If the
							// performance of the approach is too bad, we should switch to a hash.
							OBJ_Object *object = scene->objects_last;
							bool found = false;
							if (object) {
								do {
									if (0 == string_compare(object->name, tok.value)) {
										found = true;
										break;
									}
									object = object->prev;
								} while (object != NULL);
							}

							if (object == NULL || !found) {
								object = make_object(arena);
								object->name = tok.value;
								object->vertices = (OBJ_Vertex*)arena_alloc(arena, sizeof(OBJ_Vertex) * 1024LL * 1024LL);
								object->indices = (OBJ_Index*)arena_alloc(arena, sizeof(OBJ_Index) * 1024LL * 1024LL);
								append_object(scene, object);
							}

							curr_object = object;
						}
						break;
					}

					case KIND_FLOAT: {
						if (curr_keyword == KIND_KEYWORD_V) {
							positions[position_index].v[expect.count] = string_to_float(tok.value.start, (int)tok.value.len);
						} else if (curr_keyword == KIND_KEYWORD_VT) {
							tex_coords[tex_coord_index].v[expect.count] = string_to_float(tok.value.start, (int)tok.value.len);
						} else if (curr_keyword == KIND_KEYWORD_VN) {
							normals[normal_index].v[expect.count] = string_to_float(tok.value.start, (int)tok.value.len);
						} else {
							Assert(0 && "Unhandled.");
							error = true;
						}
						break;
					}

					case KIND_INTEGER: {
						break;
					}

					case KIND_PRIMITIVE_ELEMENT: {
						int first_slash = -1;
						int second_slash = -1;
						int i = 0;
						while (i < tok.value.len) {
							if (first_slash == -1 && tok.value.start[i] == '/') {
								first_slash = i;
							} else if (first_slash != -1 && tok.value.start[i] == '/') {
								second_slash = i;
								break;
							}
							i += 1;
						}

						int pe_v_index = 0;
						int pe_vt_index = 0;
						int pe_vn_index = 0;

						if (first_slash == -1) {
							// int
							pe_v_index = string_to_int(tok.value.start, (int)tok.value.len);
						} else if (first_slash != -1 && second_slash == -1) {
							// int/int
							pe_v_index = string_to_int(tok.value.start, first_slash);
							pe_vt_index = string_to_int(tok.value.start + first_slash + 1, (int)tok.value.len - first_slash - 1);
						} else if (first_slash != -1 && second_slash != -1 && first_slash + 1 == second_slash) {
							// int//int
							pe_v_index = string_to_int(tok.value.start, first_slash);
							pe_vn_index = string_to_int(tok.value.start + second_slash + 1, (int)tok.value.len - second_slash - 1);
						} else {
							// int/int/int
							pe_v_index = string_to_int(tok.value.start, first_slash);
							pe_vt_index = string_to_int(tok.value.start + first_slash + 1, second_slash - first_slash - 1);
							pe_vn_index = string_to_int(tok.value.start + second_slash + 1, (int)tok.value.len - second_slash - 1);
						}

						if (pe_v_index == 0) {
							printf("%s (%lld): Invalid vertex index in face element.", tokenizer.file_name, tokenizer.line_number);
							error = true;
						}

						// NOTE(Jan): 02/02/2025
						// Maybe make the vertex array growable?
						// TODO(Jan): Vertices are not yet correctly inserted. If a primitive element is exactly the
						// same as another one, we don't insert a new vertex. Instead, we look it up and use this index
						// in the index array. If it doesn't exist we insert it and use the new index.
						OBJ_Vertex vertex = {positions[pe_v_index], tex_coords[pe_vt_index], normals[pe_vn_index]};
						curr_object->vertices[curr_object->vertices_count++] = vertex;
						break;
					}
					default: {
						Assert(0 && "This should not happen.");
						error = true;
					}
				}
				expect.count += 1;
			} else {
				if (expect.low <= expect.count && expect.count <= expect.high) {
					next = false;

					// NOTE(Jan): When the v keyword is followed by only 3 floats the 4th value (w) must be assigned 1.
					if (expect.count < expect.high && curr_keyword == KIND_KEYWORD_V) {
						for (int i = expect.count; i < expect.high; i += 1) {
							positions[position_index].v[i] = 1.0f;
						}
					}

					expect = {1, 1, KIND_KEYWORD};

					position_index += curr_keyword == KIND_KEYWORD_V;
					tex_coord_index += curr_keyword == KIND_KEYWORD_VT;
					normal_index += curr_keyword == KIND_KEYWORD_VN;
				} else {
					printf("%s (%lld): syntax error: Expected a %s. Got: %s\n", tokenizer.file_name, tokenizer.line_number, token_kind_to_string[expect.kind], token_kind_to_string[tok.kind]);
					error = true;
				}
			}
		} else {
			printf("%s (%lld): syntax error: Expected a %s. Got: %s\n", tokenizer.file_name, tokenizer.line_number, token_kind_to_string[expect.kind], token_kind_to_string[tok.kind]);
			error = true;
		}

		//print_token(tok);
		if (next) {
			tok = next_token(&tokenizer);
		}
		error = error || tok.kind == KIND_NONE;
	}

	return {scene, tokenizer.line_number, !error && file.success};
}
