#version 130

in highp vec2 vertex2DCoord;

//out vec4 gl_Position

void main() {
	gl_Position = vec4 (vertex2DCoord, 0.0, 1.0);
}
