#version 460 core

// in
layout (location = 0) in vec4 vertColor;
// out
layout (location = 0) out vec4 fragColor;

void main() {
  fragColor = vertColor;
}