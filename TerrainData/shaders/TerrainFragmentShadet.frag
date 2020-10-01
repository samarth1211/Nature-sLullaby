#version 430 core 

layout (location = 0)out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords0;
    vec3 FragPos; 
    vec3 FragPosWorldCoord; 
    vec3 FragPosEyeCoord; 
    vec3 Normal;
    vec4 ShadowCoord;
    vec4 OutColor;
}fs_in;

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform float uXPixelOffset;
uniform float uYPixelOffset;
uniform float MaterialShininess;
uniform vec3 u_veiw_position;uniform Light SunLight;
layout(binding=0)uniform sampler2D u_texture0_bg_sampler;
layout(binding=1)uniform sampler2D u_texture1_r_sampler;
layout(binding=2)uniform sampler2D u_texture2_g_sampler;
layout(binding=3)uniform sampler2D u_texture3_b_sampler;
layout(binding=4)uniform sampler2D u_texture4_blendmap_sampler;
uniform sampler2DShadow u_texture5_depth_sampler;

void main(void)
{
    vec3 phong_ads; 
    vec3 normalized_normal = normalize(fs_in.Normal);
    vec3 view_direction=normalize(u_veiw_position-fs_in.FragPos);
    vec4 blendMapColor = texture(u_texture4_blendmap_sampler,fs_in.TexCoords0);
    float backTextureAmount = 1.0-(blendMapColor.r+blendMapColor.g+blendMapColor.b);
    vec2 tiledCoords=fs_in.TexCoords0*80;
    vec4 backgroundTextureColor = texture(u_texture0_bg_sampler,tiledCoords)*backTextureAmount;
    vec4 rTextureColor=texture(u_texture1_r_sampler,tiledCoords)*blendMapColor.r;
    vec4 gTextureColor=texture(u_texture2_g_sampler,tiledCoords)*blendMapColor.g;
    vec4 bTextureColor=texture(u_texture3_b_sampler,tiledCoords)*blendMapColor.b;
    vec4 FinalTerrainColor = backgroundTextureColor+rTextureColor+gTextureColor+bTextureColor;
    vec3 ambient = SunLight.ambient * FinalTerrainColor.rgb;
    vec3 light_direction = normalize(-SunLight.direction);
    float diffuse_multiplier = max(dot(normalized_normal,light_direction),0.0);
    vec3 diffuse = SunLight.diffuse * diffuse_multiplier * FinalTerrainColor.rgb;
    vec3 half_vector = normalize(light_direction+view_direction);
    float specular_multiplier = pow(max(dot(normalized_normal,half_vector),0.0),MaterialShininess);
    float shadow=0.0;
	//shadow = textureProj(u_texture5_depth_sampler,fs_in.ShadowCoord);
	/*if(fs_in.ShadowCoord.w > 1.0)
	{
       float x,y;
       for(y=-1.5;y<=1.5;y+=1.0)
	   {
	     for(x=-1.5;x<=1.5;x+=1.0)
		 {
		    shadow+=textureProj(u_texture5_depth_sampler,fs_in.ShadowCoord+vec4(x*uXPixelOffset*fs_in.ShadowCoord.w,y*uYPixelOffset*fs_in.ShadowCoord.w,0.05,0.0));
			shadow /= 2.0;
		 }
	   }
	}*/
	//if(fs_in.ShadowCoord.w > 1)
	{
	  float sum=0;
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(-2,-2));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(-2,0));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(-2,2));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(0,-2));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(0,0));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(0,2));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(2,-2));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(2,0));
	  sum += textureProjOffset(u_texture5_depth_sampler,fs_in.ShadowCoord,ivec2(2,2));
	  shadow = sum/9.0;
	}
    FragColor = vec4(ambient + (shadow* diffuse),1.0);
    //FragColor *= FinalTerrainColor;
}

