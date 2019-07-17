#version 330 core
out vec4 FragColor;

struct tMaterial
{
    int     isActive;
    float	shininess;
	vec3	ambient;
	vec3	diffuse;
	vec3	specular;
};

struct tLight
{
	vec3	position;
	float	intensity;
};

in GS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
}   fs_in;


uniform sampler2D texture_diffuse1;
uniform int uBackground;
uniform vec3 uCameraPosition;
uniform tLight     		uLight;
uniform tMaterial		uMaterial;
tMaterial material;

const int levels = 4;
const float scaleFactor = 1.f / levels;

float celShading(float intensity)
{
    if (intensity > 0.93f)
        return (1.f);
    else if (intensity > 0.6f)
        return (0.8f);
    else if (intensity > 0.4f)
        return (0.6f);
    else if (intensity > 0.2f)
        return (0.4f);
    else if (intensity > 0.1f)
        return (0.1f);
    return (0.f);
}

vec3	phong()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 posToLight = normalize(uLight.position - fs_in.position);
    vec3 posToCamera = normalize(uCameraPosition - fs_in.position);
    vec3 cameraToLight = normalize(posToLight + posToCamera);

    float diffuse =  max(dot(normal, posToLight), 0);
    vec3 diffuseColor = material.diffuse * celShading(diffuse);
    if (uBackground == 1)
        diffuseColor = material.diffuse;

    vec3 reflectDir = reflect(-posToLight, normal);
    float specular = pow(max(dot(posToCamera, reflectDir), 0.f), material.shininess);
    float specMask = (pow(dot(cameraToLight, normal), material.shininess) > 0.4f) ? 1.f : 0.f;

    vec3 color = (material.ambient + diffuseColor + (material.specular * specular * specMask)) * uLight.intensity;
    return (color);
}

void main()
{
    if (uMaterial.isActive == 0) {
        return ;
    }

    material = uMaterial;
    if (material.diffuse.x == material.ambient.y && material.ambient.y== material.specular.z)
        material.shininess += 0.0001f;
    vec4 color;
    color = texture(texture_diffuse1, fs_in.texCoords);
    material.diffuse = color.rgb;
    material.ambient = color.rgb;
    material.specular = color.rgb * 0.1f;
    color = vec4(phong(), 1.f);
    FragColor = vec4(color.rgb, 1.0f);
}