#pragma once
#ifndef SHADERS_H
#define SHADERS_H

static const char* shader_vert = R"(
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

static const char* shader_frag = R"(
#version 330 core

in vec2 TexCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

uniform vec3 light_color; //如果有好多光照呢
uniform vec3 object_color;

void main() {
	vec4 texColor = mix(texture(tex, TexCoord), texture(tex2, TexCoord), 0.1);
	vec4 lightingColor = vec4(light_color * object_color, 1.0);
	fragColor = mix(texColor, lightingColor, 0.5);
}
)";

static const char* shader_light_source_vert = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* shader_light_source_frag = R"(
#version 330 core

out vec4 FragColor;

uniform vec3 color;

void main() {
	FragColor = vec4(color, 1.0);
}
)";

#endif // !SHADERS_H

