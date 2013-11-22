#version 130

in highp vec2 posAttr;
in lowp vec3 colAttr;

out lowp vec3 col;

void main() {
	col = colAttr;
	gl_Position = vec4 (posAttr, 1.0, 1.0);
}
