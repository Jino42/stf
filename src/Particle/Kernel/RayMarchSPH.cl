#include "NTL.hl"


float3 rotate_float3(float3 v, float3 angle) {
    float3 n = (float3)(0.f, 0.f, 0.f);
    float3 mcos = cos(angle);
    float3 msin = sin(angle);
    float tmp = 0.f;

    n = v;
    tmp = n.y * mcos.x - n.z * msin.x;
    n.z = n.y * msin.x + n.z * mcos.x;
    n.y = tmp;
    tmp = n.x * mcos.y + n.z * msin.y;
    n.z = n.x * -msin.y + n.z * mcos.y;
    n.x = tmp;
    tmp = n.x * mcos.z - n.y * msin.z;
    n.y = n.x * msin.z + n.y * mcos.z;
    n.x = tmp;
    return (n);
}

float3 minf3(float3 a, float3 b) {
    return (float3)(a.x < b.x ? a.x : b.x,
                    a.y < b.y ? a.y : b.y,
                    a.z < b.z ? a.z : b.z);
}

float3 maxf3(float3 a, float3 b) {
    return (float3)(a.x > b.x ? a.x : b.x,
                    a.y > b.y ? a.y : b.y,
                    a.z > b.z ? a.z : b.z);
}

float opUnion(float d1, float d2) {
    return min(d1, d2);
}

float opSubtraction(float d1, float d2) {
    return max(d1, -d2);
}

float opIntersection(float d1, float d2) {
    return max(d1, d2);
}

float sdSphere(float3 position, float3 spherePosition, float sphereRadius) {
    return length(spherePosition - position) - sphereRadius;
}

float sdPlane(float3 position, float3 planePosition, float3 n) {
    return dot(planePosition - position, n.xyz);
}

float sdBox(float3 positionPoint, float3 positionBox, float3 sizeBox) {
    float3 position = positionBox - positionPoint;
    float3 q = fabs(position) - sizeBox;
    return length(maxf3(q, (float3)(0.0f, 0.0f, 0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);
}

float sdTorus(float3 positionPoint, float3 positionTorus, float2 pt) {
    float3 p = positionTorus - positionPoint;

    p = rotate_float3(p, (float3)(0.0f, 0.0f, 1.0f));

    float2 q = (float2)(length(p.xz) - pt.x, p.y);
    return length(q) - pt.y;
}

float sdBoxEmpty(float3 positionPoint, float3 positionBox, float3 sizeBox, float sizeEdge) {
    return opSubtraction(
        //sdBox(positionPoint, positionBox + (float3)(sizeEdge / 2.f, 0.0f, 0.0f), sizeBox - (float3)(sizeEdge, -sizeEdge, -sizeEdge)),
        sdBox(positionPoint, positionBox, sizeBox),
        sdSphere(positionPoint, positionBox, 15.f));
}

float opSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return mix(d2, d1, h) - k * h * (1.0f - h);
}

float sphTest(
    __global ParticleDataSPH *dataSPH,
    __global ModuleParamSPH *moduleParam,
    float3 dir,
    float3 cameraPosition,
    float t) {
    float d = FLT_MAX;

    for (int r = 0; r < 60; r++) {
        __global ParticleDataSPH *sph = &dataSPH[r];
        float tmp_d = sdSphere(cameraPosition + dir * t, sph->position, moduleParam->smoothingRadius / 2.f);

        d = min(d, tmp_d);
        d = opSmoothUnion(d, tmp_d, 0.75f);
    }
    return d;
}

float3 getCameraRayDir(float2 uv, float3 cameraPosition, float3 cameraTarget) {
    // Calculate camera's "orthonormal basis", i.e. its transform matrix components
    float3 cameraForward = normalize(cameraTarget - cameraPosition);
    float3 cameraRight = normalize(cross(cameraForward, (float3)(0.0f, 1.0f, 0.0f)));
    float3 cameraUp = normalize(cross(cameraRight, cameraForward));

    float fPersp = 2.0f;
    float3 vDir = normalize(uv.x * cameraRight + uv.y * cameraUp + cameraForward * fPersp);

    return vDir;
}

float2 normalizeScreenCoords(float2 screenCoord, float2 resolution) {
    float2 result = 2.0f * (screenCoord / resolution.xy - 0.5f);
    result.x *= resolution.x / resolution.y; // Correct for aspect ratio
    return result;
}

void kernel testKernelWrite(
    __write_only image2d_t resultTexture,
    __global ParticleDataSPH *dataSPH,
    __global ModuleParamSPH *moduleParam,
    int width,
    int height,
    float time,
    float3 cameraPosition,
    float3 cameraTarget) {

    float4 color = (float4)(0.f, 0.5f, 1.0f, 1.0f);
    float4 fcolor = (float4)(0.f, 0.5f, 1.0f, 1.0f);

    float2 uv = normalizeScreenCoords((float2)(get_global_id(0), get_global_id(1)),
                                      (float2)(width, height));
    float3 dir = getCameraRayDir(uv, cameraPosition, cameraTarget);

    float t = 0; // current distance traveled along ray

    for (int i = 0; i < 70; i++) {
        float d = FLT_MAX;

        float3 positionPoint = cameraPosition + dir * t;
        //d = sphTest(dataSPH, moduleParam, dir, cameraPosition, t);
        float3 relativePosition = positionPoint - (float3)(60.0f, 60.0f, 60.0f);
        //d = sdBox(cameraPosition + dir * t, (float3)(60.0f, 60.0f, 60.0f), (float3)(10.0f, 10.0f, 10.0f));

        float tmp;
        tmp = sdBoxEmpty(cameraPosition + dir * t, (float3)(60.0f, 60.0f, 60.0f), (float3)(10.0f, 10.0f, 10.0f), 0.1f);
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(0.8f, 0.7f, 0.0f, 1.0f);
        }
        tmp = sdPlane(cameraPosition + dir * t, (float3)(60.0f, 40.0f, 60.0f), normalize((float3)(0.2f, -1.f, 0.f)));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 0.f, 0.5f, 1.0f);
        }

        tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 60.0f), (float3)(10.f, 1.f, 10.f));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 1.f, 0.5f, 1.0f);
        }

        tmp = sdBox(cameraPosition + dir * t, (float3)(30.0f, 40.0f, 60.0f), (float3)(1.f, 3.f, 10.f));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
        }
        tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 50.0f), (float3)(100.f, 3.f, 1.f));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
        }

        tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 70.0f), (float3)(100.f, 3.f, 1.f));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
        }

        tmp = sdTorus(cameraPosition + dir * t, (float3)(50.0f, 40.0f, 60.0f), (float2)(3.0f, 1.0f));
        if (tmp < d) {
            d = tmp;
            fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
        }

        if (d < 0.0001f * t) {
            break;
        }
        t += d;
    }

    if (t < 100.f)
        color = fcolor;

    write_imagef(resultTexture, (int2)(get_global_id(0), get_global_id(1)), color);
}

void kernel testKernelWriteAA(
    __write_only image2d_t resultTexture, int width,
    int height,
    float time) {
    int2 imgCoords;
    uint work_dim = get_work_dim();
    uint global_id = get_global_id(0);
    uint global_size = get_global_size(0);
    uint local_id = get_local_id(0);
    uint local_size = get_local_size(0);
    uint group_id = get_group_id(0);
    uint n_groups = get_num_groups(0);
    imgCoords = (int2)(get_global_id(0), get_global_id(1));

    /*if (group_id == 0)
    printf("work_dim[%3i], global_id[%3i/%3i], local_id[%3i/%3i] groups_id[%3i/%3i] imgC[%i %i] wh[%i %i]\n",
            work_dim,
            global_id, global_size,
            local_id, local_size,
            group_id, n_groups,
            imgCoords.x, imgCoords.y,
            width, height);
            */
    float l = 1.0f / (float)local_size;
    float l2 = 1.0f / (float)get_local_size(1);

    float4 imgVal = (float4)(l2 * get_local_id(1), l * local_id, 0.5f, 1.0f);
    write_imagef(resultTexture, imgCoords, imgVal);
}