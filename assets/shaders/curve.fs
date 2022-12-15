#version 330 core
uniform vec3 curveColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(curveColor, 1.0);	
}