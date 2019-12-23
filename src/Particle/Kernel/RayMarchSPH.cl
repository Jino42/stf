
float sdSphere2(float3 position, float3 spherePosition, float sphereRadius) {
    return length(spherePosition - position) - sphereRadius;
}

float2 normalizeScreenCoords(float2 screenCoord, float2 resolution) {
    float2 result = 2.0f * (screenCoord / resolution.xy - 0.5f);
    result.x *= resolution.x / resolution.y; // Correct for aspect ratio
    return result;
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

void kernel testKernelWrite(
    __write_only image2d_t resultTexture,
    int width,
    int height,
    float time,
    float3 cameraPosition,
    float3 cameraTarget) {

    float4 color = (float4)(0.f, 0.5f, 1.0f, 1.0f);
    float3 position = cameraPosition;

    float2 uv = normalizeScreenCoords((float2)(get_global_id(0), get_global_id(1)),
                                      (float2)(width, height));
    float3 dir = getCameraRayDir(uv, cameraPosition, cameraTarget);




    for (int i = 0; i < 64; i++) {
        float d = sdSphere2(position, (float3)(82.5f, 258.f, 200.f), 5.f);
        if (d < 0.01f) {
            color = (float4)(1.0f, 0.f, 0.5f, 1.0f);
            break ;
        }
        position += d * dir;
    }


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