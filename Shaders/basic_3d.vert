#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord; // Assimp UVs
layout (location = 2) in vec3 aNormal; // Assimp Normals
layout (location = 3) in vec3 aTangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

uniform vec3 uColor; 
out vec3 VertexColor;

out vec3 FragPos; // World Position
out vec3 ViewPos;
out vec2 TexCoord;
out vec3 FragNormal;
out mat3 TBN; // Texture matrix


void main() {
	
	vec4 worldPosition = model * vec4(aPos, 1.0);
	FragPos = vec3(worldPosition);

	ViewPos = cameraPos;
	TexCoord = aTexCoord;

	// Normal Matrix (Handles Scaling/Rotation)
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	FragNormal = normalize(normalMatrix * aNormal); // just in case (bugfixign)

	// Transform Normal and Tangent to World Space
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);

	// Re-orthogonalize T with respect to N (Gram-Schmidt)
	T = normalize(T - dot(T, N) * N);

	// Bitangent calculation
	vec3 B = cross(N, T);

	// Create TBN Matrix
	TBN = mat3(T, B, N);

	gl_Position = projection * view * worldPosition;
}