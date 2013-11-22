#version 130

in highp vec2 vertex2DCoord;
in lowp vec3 vertexColor;

out lowp vec3 color;

void main() {
	color = vertexColor;
	gl_Position = vec4 (vertex2DCoord, 1.0, 1.0);
}
