#version 450 core

in VS_OUT
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} fs_in;

//struct for light
struct Light
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;
uniform sampler2D texture_diffuse;

uniform Light SunLight;
uniform vec3 u_view_position;

vec3 CalculateSunLight(Light DirectionalLight, vec3 TextureColor, vec3 normal, vec3 fragpos, vec3 viewdir);

void main( )
{
	vec3 phong_ads;
	vec3 normalized_normal = normalize(fs_in.Normal);
	vec3 view_direction = normalize(u_view_position - fs_in.FragPos);
	vec4 texcolor = texture( texture_diffuse, fs_in.TexCoords );
	
	if(texcolor.a < 0.1)
	{
		discard;
	}

	phong_ads += CalculateSunLight(SunLight, texcolor.rgb, normalized_normal, fs_in.FragPos, view_direction);

	FragColor = vec4(phong_ads, 1.0);
}

//function to calculate directional light
vec3 CalculateSunLight(Light SunLight, vec3 FinalTerrainColor, vec3 normalized_normal, vec3 fragpos, vec3 view_direction)
{
	// Ambient Term
	vec3 ambient = SunLight.ambient * FinalTerrainColor;

	// Diffuse Term
	vec3 light_direction = normalize(-SunLight.direction);
	float diffuse_multiplier = max(dot(normalized_normal, light_direction), 0.0);
	vec3 diffuse = SunLight.diffuse * diffuse_multiplier * FinalTerrainColor;

	// Specular Term
	vec3 half_vector = normalize(light_direction + view_direction);
	float specular_multiplier = pow(max(dot(normalized_normal, half_vector), 0.0), 50.0);
	vec3 specular = SunLight.specular * specular_multiplier * FinalTerrainColor;
	
	/*return(ambient + diffuse + specular);*/
	return(ambient + diffuse);
}
