#include "NTL.hl"

typedef struct ALIGN {
    float r; // a fraction between 0 and 1
    float g; // a fraction between 0 and 1
    float b; // a fraction between 0 and 1
} rgb;

typedef struct ALIGN {
    float h; // angle in degrees
    float s; // a fraction between 0 and 1
    float v; // a fraction between 0 and 1
} hsv;

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);

float4 hsv_to_rgb(float H, float S, float V) {
    hsv in;
    in.h = H;
    in.s = S;
    in.v = V;

    float hh, p, q, t, ff;
    long i;
    rgb out;

    if (in.s <= 0.0) {
        float4 ret;
        ret.x = in.v;
        ret.y = in.v;
        ret.z = in.v;
        ret.w = 1.f;
        return ret;
    }
    hh = in.h;
    if (hh >= 360.0f) {
        hh = 0.0f;
    }

    hh = hh / 60.0f;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0f - in.s);
    q = in.v * (1.0f - (in.s * ff));
    t = in.v * (1.0f - (in.s * (1.0f - ff)));

    switch (i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    float4 ret;
    ret.x = out.r;
    ret.y = out.g;
    ret.z = out.b;
    ret.w = 1.f;
    return ret;
}

float4 lerpColorVec4(float4 a, float4 b, float t) {
    float4 ret;

    ret.x = a.x + t * (b.x - a.x);
    ret.y = a.y + t * (b.y - a.y);
    ret.z = a.z + t * (b.z - a.z);
    ret.w = a.w + t * (b.w - a.w);

    return ret;
}

void kernel color_radius_from_position(__global ParticleData *data,
                                       float3 from,
                                       float radius,
                                       float4 colorSart,
                                       float4 colorEnd,
                                       float deltaTime) {
    uint id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    float3 velocity = particle->velocity;
    float3 position = particle->position;
    particle->color = lerpColorVec4(colorSart, colorEnd, clamp(fabs(length(from - position)) / radius, 0.f, 1.f));
}