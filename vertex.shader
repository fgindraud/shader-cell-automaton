#version 130

in highp vec4 posAttr;
in lowp vec4 colAttr;

out lowp vec4 col;

void main() {
	col = colAttr;
	gl_Position = posAttr;
}
