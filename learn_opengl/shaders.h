#pragma once
#ifndef SHADERS_H
#define SHADERS_H

const char* shader_vert = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoord;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

const char* shader_frag = R"(
#version 330 core

in vec2 TexCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
	fragColor = mix(texture(tex, TexCoord), texture(tex2, TexCoord), 0.1);
}
)";

#endif // !SHADERS_H

