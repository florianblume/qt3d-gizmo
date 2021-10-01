#version 130

in vec3 vertexPosition;
uniform mat4 modelViewProjection;

void main(void) {
    // TODO apply scaling according to camera distance
    // 1. Get the eye position
    // 2. Compute the scaling factor
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
}
