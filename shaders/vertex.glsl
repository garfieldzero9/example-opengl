#version 330 core

// Input vertex data
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// Output data
out vec3 fragmentColor;

// Uniforms
uniform mat4 MVP;
uniform float time;
uniform int anim_mode; // 0 = Cube, 1 = Wave

void main(){
    vec3 pos = vertexPosition_modelspace;
    
    if (anim_mode == 1) {
        // Wave animation
        pos.y += sin(pos.x * 4.0 + time * 2.0) * 0.25;
        pos.y += cos(pos.z * 4.0 + time * 1.5) * 0.25;
        fragmentColor = mix(vec3(0.0, 0.2, 0.8), vec3(0.0, 0.8, 0.8), (pos.y + 0.5));
    } else {
        // Default color for cube
        fragmentColor = vertexColor;
    }

    gl_Position =  MVP * vec4(pos, 1);
}