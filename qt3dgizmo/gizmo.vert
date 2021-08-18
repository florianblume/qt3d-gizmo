#version 140

in vec4 vertex;
uniform mat4 modelViewProjection;

void main(void) {
    gl_Position = modelViewProjection * vertex;
}
