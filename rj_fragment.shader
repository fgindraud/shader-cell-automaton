#version 130

uniform sampler2D prevJacobi;
//in vec4 gl_FragCoord;

//out vec4 gl_FragColor;

const float prev_ratio = 0.99;

void main (void) {
	ivec2 tex_coords = ivec2 (int (gl_FragCoord.s), int (gl_FragCoord.t));
	
	float prev_val = texelFetch (prevJacobi, tex_coords, 0).r;

	float sum_neighbours = texelFetchOffset (prevJacobi, tex_coords, 0, ivec2 (1, 0)).r;
	sum_neighbours += texelFetchOffset (prevJacobi, tex_coords, 0, ivec2 (0, -1)).r;
	sum_neighbours += texelFetchOffset (prevJacobi, tex_coords, 0, ivec2 (-1, 0)).r;
	sum_neighbours += texelFetchOffset (prevJacobi, tex_coords, 0, ivec2 (0, 1)).r;

	float new_val = mix (sum_neighbours / 4.0, prev_val, prev_ratio);

	gl_FragColor = vec4 (new_val, 0.0, 0.0, 1.0);
}
