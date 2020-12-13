#version 330 core
in vec2 TexCoords;
uniform sampler2D Texture;
out vec4 color;
void main()
{
	color = texture(Texture, TexCoords);
	color = vec4(0.7, 0.7, 0.7, 1.0);
}