#type vertex
#version 460 core

// in
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 color;
// out
layout (location = 0) out vec4 vertColor;

void main() {
    vertColor = color;
    gl_Position = pos;
}

#type fragment
#version 460 core

// in
layout (location = 0) in vec4 vertColor;
// out
layout (location = 0) out vec4 fragColor;

void main() {
    fragColor = vertColor;
}