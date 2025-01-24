#include <math.h>

union Vec2F32 {
    struct {
        F32 x;
        F32 y;
    };
    F32 v[2];
};

union Vec3F32 {
    struct {
        F32 x;
        F32 y;
        F32 z;
    };
    F32 v[3];
};

union Vec4F32 {
    struct {
        F32 x;
        F32 y;
        F32 z;
        F32 w;
    };
    F32 v[4];
};

union Vec2S32 {
    struct {
        S32 x;
        S32 y;
    };
    S32 v[2];
};

union Vec3S32 {
    struct {
        S32 x;
        S32 y;
        S32 z;
    };
    S32 v[3];
};

union Vec4S32 {
    struct {
        S32 x;
        S32 y;
        S32 z;
        S32 w;
    };
    S32 v[4];
};

union Vec2U32 {
    struct {
        U32 x;
        U32 y;
    };
    U32 v[2];
};

union Vec3U32 {
    struct {
        U32 x;
        U32 y;
        U32 z;
    };
    U32 v[3];
};

union Vec4U32 {
    struct {
        U32 x;
        U32 y;
        U32 z;
        U32 w;
    };
    U32 v[4];
};

struct Mat3F32 {
    F32 v[3][3];
};

struct Mat4F32 {
    F32 v[4][4];
};

#define PI 3.14159265359f
#define TABLE_SIZE 257
#define STEP_SIZE 0.25f * 2 * PI / (TABLE_SIZE - 1)

F32 table[TABLE_SIZE] = {
    0.0000000f, 0.0061359f, 0.0122715f, 0.0184067f, 0.0245412f, 0.0306748f,
    0.0368072f, 0.0429383f, 0.0490677f, 0.0551952f, 0.0613207f, 0.0674439f,
    0.0735646f, 0.0796824f, 0.0857973f, 0.0919090f, 0.0980171f, 0.1041216f,
    0.1102222f, 0.1163186f, 0.1224107f, 0.1284981f, 0.1345807f, 0.1406582f,
    0.1467305f, 0.1527972f, 0.1588582f, 0.1649131f, 0.1709619f, 0.1770042f,
    0.1830399f, 0.1890687f, 0.1950903f, 0.2011046f, 0.2071114f, 0.2131103f,
    0.2191012f, 0.2250839f, 0.2310581f, 0.2370236f, 0.2429802f, 0.2489276f,
    0.2548656f, 0.2607941f, 0.2667128f, 0.2726214f, 0.2785197f, 0.2844076f,
    0.2902847f, 0.2961509f, 0.3020059f, 0.3078496f, 0.3136818f, 0.3195020f,
    0.3253103f, 0.3311063f, 0.3368898f, 0.3426607f, 0.3484187f, 0.3541635f,
    0.3598951f, 0.3656130f, 0.3713172f, 0.3770074f, 0.3826835f, 0.3883450f,
    0.3939920f, 0.3996242f, 0.4052413f, 0.4108432f, 0.4164295f, 0.4220003f,
    0.4275551f, 0.4330938f, 0.4386162f, 0.4441221f, 0.4496113f, 0.4550836f,
    0.4605387f, 0.4659765f, 0.4713967f, 0.4767992f, 0.4821838f, 0.4875502f,
    0.4928982f, 0.4982277f, 0.5035384f, 0.5088302f, 0.5141027f, 0.5193560f,
    0.5245897f, 0.5298036f, 0.5349976f, 0.5401715f, 0.5453250f, 0.5504580f,
    0.5555702f, 0.5606616f, 0.5657318f, 0.5707808f, 0.5758082f, 0.5808140f,
    0.5857978f, 0.5907597f, 0.5956993f, 0.6006165f, 0.6055110f, 0.6103828f,
    0.6152316f, 0.6200572f, 0.6248595f, 0.6296383f, 0.6343933f, 0.6391245f,
    0.6438316f, 0.6485144f, 0.6531729f, 0.6578067f, 0.6624158f, 0.6669999f,
    0.6715590f, 0.6760927f, 0.6806010f, 0.6850837f, 0.6895406f, 0.6939715f,
    0.6983762f, 0.7027547f, 0.7071068f, 0.7114322f, 0.7157308f, 0.7200025f,
    0.7242470f, 0.7284644f, 0.7326543f, 0.7368166f, 0.7409511f, 0.7450578f,
    0.7491364f, 0.7531868f, 0.7572088f, 0.7612024f, 0.7651673f, 0.7691033f,
    0.7730104f, 0.7768885f, 0.7807372f, 0.7845566f, 0.7883464f, 0.7921066f,
    0.7958369f, 0.7995373f, 0.8032075f, 0.8068475f, 0.8104572f, 0.8140363f,
    0.8175848f, 0.8211025f, 0.8245893f, 0.8280451f, 0.8314696f, 0.8348629f,
    0.8382247f, 0.8415549f, 0.8448536f, 0.8481203f, 0.8513552f, 0.8545580f,
    0.8577286f, 0.8608669f, 0.8639728f, 0.8670462f, 0.8700870f, 0.8730950f,
    0.8760701f, 0.8790122f, 0.8819212f, 0.8847971f, 0.8876396f, 0.8904487f,
    0.8932243f, 0.8959663f, 0.8986744f, 0.9013488f, 0.9039893f, 0.9065957f,
    0.9091680f, 0.9117060f, 0.9142098f, 0.9166790f, 0.9191138f, 0.9215140f,
    0.9238795f, 0.9262102f, 0.9285061f, 0.9307670f, 0.9329928f, 0.9351835f,
    0.9373390f, 0.9394592f, 0.9415441f, 0.9435934f, 0.9456074f, 0.9475856f,
    0.9495282f, 0.9514350f, 0.9533060f, 0.9551412f, 0.9569404f, 0.9587035f,
    0.9604305f, 0.9621214f, 0.9637761f, 0.9653944f, 0.9669765f, 0.9685221f,
    0.9700313f, 0.9715039f, 0.9729400f, 0.9743394f, 0.9757021f, 0.9770281f,
    0.9783174f, 0.9795697f, 0.9807853f, 0.9819639f, 0.9831055f, 0.9842101f,
    0.9852777f, 0.9863081f, 0.9873014f, 0.9882576f, 0.9891765f, 0.9900582f,
    0.9909027f, 0.9917098f, 0.9924796f, 0.9932119f, 0.9939070f, 0.9945646f,
    0.9951847f, 0.9957674f, 0.9963126f, 0.9968203f, 0.9972904f, 0.9977230f,
    0.9981181f, 0.9984756f, 0.9987954f, 0.9990777f, 0.9993224f, 0.9995294f,
    0.9996988f, 0.9998306f, 0.9999247f, 0.9999812f, 1.0000000f
};

//
// Basic math operations.
F32 sqrt_f32(F32 value);
F32 ceil_f32(F32 value);
F32 floor_f32(F32 value);
F32 round_f32(F32 val);

//
// Trigonometric functions
F32 sin_f32(F32 turn);
F32 cos_f32(F32 turn);
F32 tan_f32(F32 turn);
F32 cot_f32(F32 turn);

//
// Vec2F32 operations
Vec2F32 operator+(Vec2F32 v1, Vec2F32 v2);
Vec2F32 operator-(Vec2F32 v1, Vec2F32 v2);
Vec2F32 operator*(Vec2F32 v1, Vec2F32 v2);
Vec2F32 operator/(Vec2F32 v1, Vec2F32 v2);
F32 dot_2f32(Vec2F32 v1, Vec2F32 v2);
F32 len_2f32(Vec2F32 v);
Vec2F32 normalize_2f32(Vec2F32 v);
void normalize_this_2f32(Vec2F32 *v);
Vec2F32 operator*(Vec2F32 v, F32 s);
void scale_this_2f32(Vec2F32 *v, F32 s);

//
// Vec3F32 operations.
Vec3F32 operator+(Vec3F32 v1, Vec3F32 v2);
Vec3F32 operator-(Vec3F32 v1, Vec3F32 v2);
Vec3F32 operator*(Vec3F32 v1, Vec3F32 v2);
Vec3F32 operator/(Vec3F32 v1, Vec3F32 v2);
F32 dot_3f32(Vec3F32 v1, Vec3F32 v2);
F32 len_3f32(Vec3F32 v);
Vec3F32 normalize_3f32(Vec3F32 v);
void normalize_this_3f32(Vec3F32 *v);
Vec3F32 cross_3f32(Vec3F32 v1, Vec3F32 v2);
Vec3F32 operator*(Vec3F32 v, F32 s);
void scale_this_3f32(Vec3F32 *v, F32 s);

//
// Vec4F32 operations.
Vec4F32 operator+(Vec4F32 v1, Vec4F32 v2);
Vec4F32 operator-(Vec4F32 v1, Vec4F32 v2);
Vec4F32 operator*(Vec4F32 v1, Vec4F32 v2);
Vec4F32 operator/(Vec4F32 v1, Vec4F32 v2);
F32 dot_4f32(Vec4F32 v1, Vec4F32 v2);
F32 len_4f32(Vec4F32 v);
Vec4F32 normalize_4f32(Vec4F32 v);
void normalize_this_4f32(Vec4F32 *v);
Vec4F32 cross_4f32(Vec4F32 v1, Vec4F32 v2);
Vec4F32 operator*(Vec4F32 v, F32 s);
void scale_this_4f32(Vec4F32 *v, F32 s);

//
// F32 matrix 4 by 4 operations.
Mat4F32 identity_mat4f32(F32 value);
Mat4F32 transpose_mat4f32(Mat4F32 m);
void transpose_this_mat4f32(Mat4F32 *m);
Mat4F32 operator+(Mat4F32 m1, Mat4F32 m2);
Mat4F32 operator-(Mat4F32 m1, Mat4F32 m2);
Mat4F32 operator*(Mat4F32 m1, Mat4F32 m2);

//
// F32 matrix vector operations
Vec4F32 operator*(Mat4F32 m, Vec4F32 v);

//
// transformations
Mat4F32 look_at(Vec3F32 position, Vec3F32 target, Vec3F32 fake_up);
Mat4F32 orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 znear, F32 zfar);
Mat4F32 perspective(F32 fov, F32 aspect, F32 znear, F32 zfar);
Mat4F32 perspective2(F32 fov, F32 aspect, F32 znear, F32 zfar);

// Basic math operations.
F32 sqrt_f32(F32 value) {
    return sqrtf(value);
}

F32 ceil_f32(F32 value) {
    int truncated = (int)value;
    return (F32)(truncated + (value > truncated));
}

F32 floor_f32(F32 value) {
    int truncated = (int)value;
    return (F32)(truncated - (value < truncated));
}

F32 round_f32(F32 val) {
    // if val is positive 1 - 0 = +1,
    // if val is negative 0 - 1 = -1.
    F32 half = 0.5;
    return (F32)(int)(val + half * Sign(val));
}

//
// trigonometric functions
F32 sin_f32(F32 turn) {
    F32 normalized_turn = turn - floor_f32(turn); /* keep turn in range 0..1 */

    int mirror = 0;
    int flip = 1;

    F32 index;
    if (normalized_turn >= 0.0f && normalized_turn < 0.25f) {
        index = normalized_turn * 4.0f * (TABLE_SIZE - 1);
    }
    else if (normalized_turn >= 0.25f && normalized_turn < 0.5f) {
        index = (normalized_turn - 0.25f) * 4.0f * (TABLE_SIZE - 1);
        mirror = 1;
    }
    else if (normalized_turn >= 0.5f && normalized_turn < 0.75f) {
        index = (normalized_turn - 0.5f) * 4.0f * (TABLE_SIZE - 1);
        flip = -1;
    }
    else {
        index = (normalized_turn - 0.75f) * 4.0f * (TABLE_SIZE - 1);
        mirror = 1;
        flip = -1;
    }
    if (mirror) {
        index = (TABLE_SIZE - 1) - index;
    }
    int index0 = (int)index;
    int index1 = index0 + 1;

    F32 lerp = table[index0] + (((table[index1] - table[index0]) / STEP_SIZE) *
                                  ((index - index0) * STEP_SIZE));

    return flip * lerp;
}

F32 cos_f32(F32 turn) {
    F32 normalized_turn = turn - floor_f32(turn); /* keep turn in range 0..1 */

    int mirror = 0;
    int flip = 0;

    F32 index;
    if (normalized_turn >= 0.0f && normalized_turn < 0.25f) {
        index = normalized_turn * 4.0f * (TABLE_SIZE - 1);
        mirror = 1;
    }
    else if (normalized_turn >= 0.25f && normalized_turn < 0.5f) {
        index = (normalized_turn - 0.25f) * 4.0f * (TABLE_SIZE - 1);
        flip = 1;
    }
    else if (normalized_turn >= 0.5f && normalized_turn < 0.75f) {
        index = (normalized_turn - 0.5f) * 4.0f * (TABLE_SIZE - 1);
        mirror = 1;
        flip = 1;
    }
    else {
        index = (normalized_turn - 0.75f) * 4.0f * (TABLE_SIZE - 1);
    }
    if (mirror) {
        index = (TABLE_SIZE - 1) - index;
    }
    int index0 = (int)index;
    int index1 = index0 + 1;

    F32 lerp = table[index0] + (((table[index1] - table[index0]) / STEP_SIZE) *
                                  ((index - index0) * STEP_SIZE));

    if (flip) {
        return -lerp;
    }
    else {
        return lerp;
    }
}

F32 tan_f32(F32 turn) {
    return sin_f32(turn) / cos_f32(turn);
}

F32 cot_f32(F32 turn) {
    return cos_f32(turn) / sin_f32(turn);
}

//
// F32 vector 2
Vec2F32 operator+(Vec2F32 v1, Vec2F32 v2) {
    return {v1.x + v2.x, v1.y + v2.y};
}

Vec2F32 operator-(Vec2F32 v1, Vec2F32 v2) {
    return {v1.x - v2.x, v1.y - v2.y};
}

Vec2F32 operator*(Vec2F32 v1, Vec2F32 v2) {
    return {v1.x * v2.x, v1.y * v2.y};
}

Vec2F32 operator/(Vec2F32 v1, Vec2F32 v2) {
    return {v1.x / v2.x, v1.y / v2.y};
}

F32 dot_2f32(Vec2F32 v1, Vec2F32 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

F32 len_2f32(Vec2F32 v) {
    return sqrt_f32(dot_2f32(v, v));
}

Vec2F32 normalize_2f32(Vec2F32 v) {
    F32 len = len_2f32(v);
    return {v.x / len, v.y / len};
}

void normalize_this_2f32(Vec2F32 *v) {
    F32 len = len_2f32(*v);
    v->x /= len;
    v->y /= len;
}

Vec2F32 operator*(Vec2F32 v, F32 s) {
    return {v.x * s, v.y * s};
}

void vec2_scale_this(Vec2F32 *v, F32 s) {
    v->x *= s;
    v->y *= s;
}

//
// F32 vector 3
Vec3F32 operator+(Vec3F32 v1, Vec3F32 v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vec3F32 operator-(Vec3F32 v1, Vec3F32 v2) {
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

Vec3F32 operator*(Vec3F32 v1, Vec3F32 v2) {
    return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

Vec3F32 operator/(Vec3F32 v1, Vec3F32 v2) {
    return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

F32 dot_3f32(Vec3F32 v1, Vec3F32 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

F32 len_3f32(Vec3F32 v) {
    return sqrt_f32(dot_3f32(v, v));
}

Vec3F32 normalize_3f32(Vec3F32 v) {
    F32 len = len_3f32(v);
    return {v.x / len, v.y / len, v.z / len};
}

void normalize_this_3f32(Vec3F32 *v) {
    F32 len = len_3f32(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

Vec3F32 cross_3f32(Vec3F32 v1, Vec3F32 v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x,
    };
}

Vec3F32 operator*(Vec3F32 v, F32 s) {
    return {v.x * s, v.y * s, v.z * s};
}

void scale_this_3f32(Vec3F32 *v, F32 s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

//
// F32 vector 4
Vec4F32 operator+(Vec4F32 v1, Vec4F32 v2) {
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

Vec4F32 operator-(Vec4F32 v1, Vec4F32 v2) {
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

Vec4F32 operator*(Vec4F32 v1, Vec4F32 v2) {
    return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
}

Vec4F32 operator/(Vec4F32 v1, Vec4F32 v2) {
    return {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w};
}

F32 dot_4f32(Vec4F32 v1, Vec4F32 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

F32 len_4f32(Vec4F32 v) {
    return sqrt_f32(dot_4f32(v, v));
}

Vec4F32 normalize_4f32(Vec4F32 v) {
    F32 len = len_4f32(v);
    return {v.x / len, v.y / len, v.z / len, v.w / len};
}

void normalize_this_4f32(Vec4F32 *v) {
    F32 len = len_4f32(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
    v->w /= len;
}

Vec4F32 operator*(Vec4F32 v, F32 s) {
    return {v.x * s, v.y * s, v.z * s, v.w * s};
}

void scale_this_4f32(Vec4F32 *v, F32 s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
    v->w *= s;
}

//
// F32 matrix 4 by 4
Mat4F32 identity_mat4f32(F32 value) {
    Mat4F32 m;
    m.v[0][0] = value, m.v[0][1] = 0.0f,  m.v[0][2] = 0.0f,  m.v[0][3] = 0.0f;
    m.v[1][0] = 0.0f,  m.v[1][1] = value, m.v[1][2] = 0.0f,  m.v[1][3] = 0.0f;
    m.v[2][0] = 0.0f,  m.v[2][1] = 0.0f,  m.v[2][2] = value, m.v[2][3] = 0.0f;
    m.v[3][0] = 0.0f,  m.v[3][1] = 0.0f,  m.v[3][2] = 0.0f,  m.v[3][3] = value;
    return m;
}

Mat4F32 transpose_mat4f32(Mat4F32 m_in) {
    Mat4F32 m_out;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m_out.v[j][i] = m_in.v[i][j];
        }
    }
    return m_out;
}

void transpose_this_mat4f32(Mat4F32 *m) {
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            F32 f = m->v[i][j];
            m->v[i][j] = m->v[j][i];
            m->v[j][i] = f;
        }
    }
}

Mat4F32 operator+(Mat4F32 m1, Mat4F32 m2) {
    Mat4F32 out;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            out.v[row][column] = m1.v[row][column] + m2.v[row][column];
        }
    }
    return out;
}


Mat4F32 operator-(Mat4F32 m1, Mat4F32 m2) {
    Mat4F32 out;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            out.v[row][column] = m1.v[row][column] - m2.v[row][column];
        }
    }
    return out;
}

Mat4F32 operator*(Mat4F32 m1, Mat4F32 m2) {
    Mat4F32 out;
    for (int i = 0; i < 4; ++i) { /* row of m1 */
        for (int j = 0; j < 4; ++j) { /* column of m2 */
            out.v[j][i] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                out.v[j][i] += m1.v[k][i] * m2.v[j][k];
            }
        }
    }
    return out;
}

//
// F32 matrix vector operations
Vec4F32 operator*(Mat4F32 m, Vec4F32 v) {
    Vec4F32 out;
    for (int i = 0; i < 4; ++i) { // row of m
        out.v[i] = 0.0f;
        for (int k = 0; k < 4; ++k) {
            out.v[i] += m.v[k][i] * v.v[k];
        }
    }
    return out;
}

//
// transformations
Mat4F32 look_at(Vec3F32 position, Vec3F32 target, Vec3F32 fake_up) {
    Vec3F32 backward, right, up;

    backward = operator-(position, target);
    right = cross_3f32(fake_up, backward);
    up = cross_3f32(backward, right);

    normalize_this_3f32(&backward);
    normalize_this_3f32(&right);
    normalize_this_3f32(&up);

    F32 cos1, cos2, cos3;

    cos1 = dot_3f32(right, position);
    cos2 = dot_3f32(up, position);
    cos3 = dot_3f32(backward, position);

    Mat4F32 out;
    out.v[0][0] = right.v[0], out.v[0][1] = up.v[0], out.v[0][2] = backward.v[0], out.v[0][3] = 0.0f;
    out.v[1][0] = right.v[1], out.v[1][1] = up.v[1], out.v[1][2] = backward.v[1], out.v[1][3] = 0.0f;
    out.v[2][0] = right.v[2], out.v[2][1] = up.v[2], out.v[2][2] = backward.v[2], out.v[2][3] = 0.0f;
    out.v[3][0] = -cos1,      out.v[3][1] = -cos2,   out.v[3][2] = -cos3,         out.v[3][3] = 1.0f;
    return out;
}

Mat4F32 orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 znear, F32 zfar) {
    F32 width = right - left;
    F32 height = top - bottom;
    F32 depth = zfar - znear;

    F32 m30 = (right + left) / width;
    F32 m31 = (top + bottom) / height;
    F32 m32 = (zfar + znear) / depth;

    Mat4F32 out;
    out.v[0][0] = 2.0f / width,  out.v[0][1] = 0.0f,          out.v[0][2] = 0.0f,          out.v[0][3] = 0.0f;
    out.v[1][0] = 0.0f,          out.v[1][1] = 2.0f / height, out.v[1][2] = 0.0f,          out.v[1][3] = 0.0f;
    out.v[2][0] = 0.0f,          out.v[2][1] = 0.0f,          out.v[2][2] = -2.0f / depth, out.v[2][3] = 0.0f;
    out.v[3][0] = -m30,          out.v[3][1] = -m31,          out.v[3][2] = -m32,          out.v[3][3] = 1.0f;
    return out;
}

Mat4F32 perspective(F32 fov, F32 aspect, F32 znear, F32 zfar) {
    F32 f = cot_f32(0.5f * fov);
    F32 inv_depth = 1.0f / (znear - zfar);

    Mat4F32 out;
    out.v[0][0] = f * aspect, out.v[0][1] = 0.0f, out.v[0][2] = 0.0f,                         out.v[0][3] =  0.0f;
    out.v[1][0] = 0.0f,       out.v[1][1] = f,    out.v[1][2] = 0.0f,                         out.v[1][3] =  0.0f;
    out.v[2][0] = 0.0f,       out.v[2][1] = 0.0f, out.v[2][2] = (zfar + znear) * inv_depth,   out.v[2][3] = -1.0f;
    out.v[3][0] = 0.0f,       out.v[3][1] = 0.0f, out.v[3][2] = 2 * zfar * znear * inv_depth, out.v[3][3] =  0.0f;
    return out;
}

Mat4F32 perspective2(F32 fov, F32 aspect, F32 znear, F32 zfar) {
    F32 right = znear * tan_f32(fov * 0.5f);
    F32 top = right * aspect;
    F32 inv_d = 1.0f / (zfar - znear);

    Mat4F32 out;
    out.v[0][0] = znear / right, out.v[0][1] = 0.0f,        out.v[0][2] = 0.0f,                      out.v[0][3] =  0.0f;
    out.v[1][0] = 0.0f,          out.v[1][1] = znear / top, out.v[1][2] = 0.0f,                      out.v[1][3] =  0.0f;
    out.v[2][0] = 0.0f,          out.v[2][1] = 0.0f,        out.v[2][2] = -(zfar + znear) * inv_d,   out.v[2][3] = -1.0f;
    out.v[3][0] = 0.0f,          out.v[3][1] = 0.0f,        out.v[3][2] = -2 * zfar * znear * inv_d, out.v[3][3] =  0.0f;
    return out;
}
