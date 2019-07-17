/*#include "ParticleData.hl"
#include "Tools.hl"

typedef struct {
    float r;       // a fraction between 0 and 1
    float g;       // a fraction between 0 and 1
    float b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    float h;       // angle in degrees
    float s;       // a fraction between 0 and 1
    float v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

vec4 hsv_to_rgb(float H, float S, float V)
{
    hsv in;
    in.h = H;
    in.s = S;
    in.v = V;

    float      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {
        vec4 ret;
        ret.x = in.v;
        ret.y = in.v;
        ret.z = in.v;
        ret.w = 1.f;
        return ret;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
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
    vec4 ret;
    ret.x = out.r;
    ret.y = out.g;
    ret.z = out.b;
    ret.w = 1.f;
    return ret;
}

vec4 lerpColorVec4 (vec4 a, vec4 b, float t)
{
    vec4 ret;
    ret.x = a.x + (b.x - a.x) * t;
    ret.y = a.y + (b.y - a.y) * t;
    ret.z = a.z + (b.z - a.z) * t;
    ret.w = a.w + (b.w - a.w) * t;

    return ret;
}

void kernel color_radius_from_position(__global ParticleData *data,
                        float3 from,
                        float radius,
                        float deltaTime) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    vec4 end = hsv_to_rgb(0.f, 0.93f, 0.84f);
    vec4 start = hsv_to_rgb(60.f, 1.f, 0.92f);

    float3 position;
    position.x = particle->position.x;
    position.y = particle->position.y;
    position.z = particle->position.z;
    particle->color = lerpColorVec4(start, end, clamp(fabs(length(from - position)) / radius, 0.f, 1.f));
}
*/