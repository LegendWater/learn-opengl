#pragma once
#ifndef SHADERS_H
#define SHADERS_H

const char* shader_vert = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform float coefficient;

out vec2 TexCoord;

void main() {
	gl_Position = vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

const char* shader_frag = R"(
#version 330 core

in vec2 TexCoord;
out vec4 fragColor;

uniform float coefficient;
uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
	fragColor = mix(texture(tex, TexCoord), texture(tex2, TexCoord), 1.0);
	//fragColor.x = mix(mix(fragColor.x, fragColor.y, cos(coefficient)), fragColor.z, coefficient);
	//fragColor.y = mix(mix(fragColor.y, fragColor.z, cos(1 - coefficient)), fragColor.x, 1 - coefficient);
	//fragColor.z = mix(mix(fragColor.z, fragColor.x, cos(0.5 * coefficient)), fragColor.y, 0.5 * coefficient);
}
)";

#endif // !SHADERS_H

