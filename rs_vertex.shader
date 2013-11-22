#version 130

in vec2 vertex_coords;

//out vec4 gl_Position;

void main (void) {
	gl_Position = vec4 (vertex_coords, 0.0, 1.0);
}
