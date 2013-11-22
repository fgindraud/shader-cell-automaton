#version 130

uniform sampler2D jacobi;
uniform float width;
uniform float height;
//in vec4 gl_FragCoord;

//out vec4 gl_FragColor;

void main () {
	vec2 pos = vec2 (gl_FragCoord.x / width, gl_FragCoord.y / height);
	float v = texture (jacobi, pos).r;

	gl_FragColor = vec4 (v, v, v, 1.0);
}
