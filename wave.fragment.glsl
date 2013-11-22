#version 130

uniform sampler2D prev_jacobi;
//in vec4 gl_FragCoord;

//out vec4 gl_FragColor;

const float prev_ratio = 0.1;

void main (void) {
	ivec2 tex_coords = ivec2 (int (gl_FragCoord.s), int (gl_FragCoord.t));
	
	float pos = texelFetch (prev_jacobi, tex_coords, 0).r;
	float vit = texelFetch (prev_jacobi, tex_coords, 0).g;

	float acc = -4*texelFetch (prev_jacobi, tex_coords, 0).r;
	acc += texelFetchOffset (prev_jacobi, tex_coords, 0, ivec2 (-1,  0)).r;
	acc += texelFetchOffset (prev_jacobi, tex_coords, 0, ivec2 ( 1,  0)).r;
	acc += texelFetchOffset (prev_jacobi, tex_coords, 0, ivec2 ( 0, -1)).r;
	acc += texelFetchOffset (prev_jacobi, tex_coords, 0, ivec2 ( 0,  1)).r;
	acc /= 4;

	//vit *= 0.95;
	vit += acc;

	pos += vit;

	gl_FragColor = vec4 (pos, vit, 0.0, 1.0);
}
