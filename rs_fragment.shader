#version 130

uniform sampler2D jacobi_texture;
uniform float width;
uniform float height;
//in vec4 gl_FragCoord;

//out vec4 gl_FragColor;

void main (void) {
	vec2 pos = vec2 (gl_FragCoord.x / width, gl_FragCoord.y / height);
	
	float p = texture (jacobi_texture, pos).r;
	float v = texture (jacobi_texture, pos).g;

	gl_FragColor = vec4 (clamp(p, 0.0, 1.0), clamp(v, 0.0, 1.0), clamp(-v, 0.0, 1.0), 1.0);
	//gl_FragColor = vec4 (p, p, p, 1.0);
}
