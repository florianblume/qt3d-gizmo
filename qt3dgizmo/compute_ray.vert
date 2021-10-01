#version 430 core

// Simply do all in one call
layout (local_size_x=1) in;

layout (std430, binding = 1) buffer rays {
    vec3 result[2];
};

// These are automatically provided by Qt3D
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewportMatrix;
uniform vec3 eyePosition;

// The current mouse coordinates we need a ray for
uniform int mouseX;
uniform int mouseY;

struct Viewport {
    // x and y are irrelevant for us
    int x;
    int y;
    int width;
    int height;
};

vec3 unproject(in vec3 vector, in mat4 viewMatrix, in mat4 projectionMatrix, in Viewport viewport) {
    mat4 inverseProjectionViewMatrix = inverse(projectionMatrix * viewMatrix);

    vec4 tmp = vec4(vector, 1.0f);
    tmp[0] = ((tmp.x - float(viewport.x)) / float(viewport.width));
    tmp[1] =((tmp.y - float(viewport.y)) / float(viewport.height));
    tmp = tmp * 2.0f - vec4(1.0f);

    vec4 obj = inverseProjectionViewMatrix * tmp;
    if (obj.w < 0.0001 || obj.w > -0.0001) {
        obj.w = 1.0f;
    }
    obj = obj / obj.w;
    return vec3(obj);
}

void main() {
    // TODO compute ray
    // Get mouse x and y coordinates
    int x = mouseX;
    int y = mouseY;

    Viewport viewport;
    viewport.width = int(viewportMatrix[0][0]) * 2;
    viewport.height = int(viewportMatrix[1][1]) * 2;

    float winZ0 = 0.0f;
    float winZ1 = 1.0f;
    int yCorrected = viewport.height - y;

    vec3 projectedStart = vec3(x, yCorrected, winZ0);
    vec3 projectedEnd = vec3(x, yCorrected, winZ1);


    // TODO: Compute WindowSize using Viewport
    vec3 unprojectedStart = unproject(projectedStart,
                                      viewMatrix,
                                      projectionMatrix,
                                      viewport);
    vec3 unprojectedEnd = unproject(projectedEnd,
                                    viewMatrix,
                                    projectionMatrix,
                                    viewport);
    result[0] = unprojectedStart;
    result[1] = unprojectedEnd;
}
