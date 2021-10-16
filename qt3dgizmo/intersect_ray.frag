#version 430 core

// Simply do all computations in one call (x=1 is only one thread)
layout (local_size_x=1) in;

layout (std430, binding = 1) buffer result {
    float intersection[];
};

// These are automatically provided by Qt3D
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewportMatrix;
uniform vec3 eyePosition;

uniform bool computeRequested;

uniform float vertices[1000];
uniform int verticesCount;
uniform int vertexByteStride;
uniform int vertexByteOffset;

uniform int indices[1000];
uniform int indicesCount;

// The current mouse coordinates we need a ray for
uniform int mouseX;
uniform int mouseY;

struct Viewport {
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

void computeIntersection(in vec3 a, in vec3 b, in vec3 c, in vec3 start, in vec3 end, out bool intersected, out vec3 worldIntersection) {
    // Qt3D's internal triangle intersection procedure
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 qp = (start - end);

    vec3 n = cross(ab, ac);
    const float d = dot(qp, n);

    if (d <= 0.0f) {
        intersected = false;
        return;
    }

    vec3 ap = start - a;
    float t = dot(ap, n);

    if (t < 0.0f || t > d) {
        intersected = false;
        return;
    }

    vec3 e = cross(qp, ap);
    vec3 uvw;
    uvw.y = dot(ac, e);

    if (uvw.y < 0.0f || uvw.y > d) {
        intersected = false;
        return;
    }

    uvw.z = -dot(ab, e);

    if (uvw.z < 0.0f || uvw.y + uvw.z > d) {
        intersected = false;
        return;
    }

    float ood = 1.0f / d;
    t *= ood;
    uvw.y = uvw.y * ood;
    uvw.z = uvw.z * ood;
    uvw.x = 1.0f - uvw.y - uvw.z;

    worldIntersection = start + t * (end - start);
    intersected = true;
}

void main() {
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

    vec3 unprojectedStart = unproject(projectedStart,
                                      viewMatrix,
                                      projectionMatrix,
                                      viewport);
    vec3 unprojectedEnd = unproject(projectedEnd,
                                    viewMatrix,
                                    projectionMatrix,
                                    viewport);
    bool intersected = false;
    vec3 worldIntersection = vec3(0);

    for (int i = 0; i < indicesCount; i += 3) {
        int idx0 = indices[i + 0];
        int idx1 = indices[i + 1];
        int idx2 = indices[i + 2];
        vec3 a = vec3(vertices[idx0 * vertexByteStride + vertexByteOffset + 0],
                      vertices[idx0 * vertexByteStride + vertexByteOffset + 1],
                      vertices[idx0 * vertexByteStride + vertexByteOffset + 2]);
        vec3 b = vec3(vertices[idx1 * vertexByteStride + vertexByteOffset + 0],
                      vertices[idx1 * vertexByteStride + vertexByteOffset + 1],
                      vertices[idx1 * vertexByteStride + vertexByteOffset + 2]);
        vec3 c = vec3(vertices[idx2 * vertexByteStride + vertexByteOffset + 0],
                      vertices[idx2 * vertexByteStride + vertexByteOffset + 1],
                      vertices[idx2 * vertexByteStride + vertexByteOffset + 2]);
        // need to multiple vertex coordinates with modelView matrix
        computeIntersection(a, b, c, unprojectedStart, unprojectedEnd, intersected, worldIntersection);
        if (intersected) {
            intersection[0] = worldIntersection[0];
            intersection[1] = worldIntersection[1];
            intersection[2] = worldIntersection[2];
            vec4 localIntersection = inverse(modelMatrix) * inverse(viewMatrix) * vec4(worldIntersection, 1.0);
            intersection[4] = localIntersection[0];
            intersection[5] = localIntersection[1];
            intersection[6] = localIntersection[2];
        }
    }
}
