#version 330

in vec2 fragUV;

//uniform vec3 materialDiffuse;
//uniform vec3 materialSpecular;

out vec4 Color;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;

void main(void)
{
	vec2 texCoords = vec2(fragUV.x, 1.0 - fragUV.y);
	vec4 diffuseTextureColor = texture(DiffuseTexture, texCoords);
	vec4 specularTextureColor = texture(SpecularTexture, texCoords);
	
	vec3 diffuse = diffuseTextureColor.rgb;
	
	vec3 specular = specularTextureColor.rgb;
	
	float alpha = diffuseTextureColor.a + specularTextureColor.a;
	
	vec3 finalColor = (diffuse + specular);
	Color = vec4(finalColor, alpha);
}