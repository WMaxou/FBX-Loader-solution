#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

mat4 View();
mat4 Proj();
mat4 Model();

out vec2 fragUV;

void main(void)
{
	fragUV = texCoords;
	
	//colorOut.specular = texSpecular;
	
	gl_Position = Proj() * View() * Model() * vec4(pos, 1.0);
}

mat4 View()
{
	return mat4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
}

mat4 Model()
{
	return mat4(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, -40.0, -120.0, 1);
}

mat4 Proj()
{
	float width = 1280.0; float height = 720.0;
	float aspectRatio = width / height;		// pour que les pixels soient "carres"
	float fovY = radians(45.0);				// degree d'ouverture = field of view
	float near = 0.1;						// distance minimale avant laquelle on "clip" le rendu
	float far = 1000.0;						// distance maximale apres laquelle on "clip" le rendu
	
	float d = 1.0 / tan(fovY/2.0);			// distance focale (oeil->plan de projection)
	float range = 1.0 / (near - far);		// normalisation en NDC de la profondeur
	mat4 projectionMatrix = mat4(
						d / aspectRatio, 0.0, 0.0, 0.0,
						0.0, d, 0.0, 0.0,
						0.0, 0.0, (near+far)*range, -1.0,
						0.0, 0.0, (near*far*2.0)*range, 0.0
					);

	return projectionMatrix;
}