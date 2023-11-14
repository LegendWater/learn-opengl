#pragma once
#ifndef SHADERS_H
#define SHADERS_H

static const char* shader_vert = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 worldFragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	worldFragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Normal = mat3(transpose(inverse(model))) * aNormal; //TODO 把法线矩阵交给cpu来算
	TexCoord = aTexCoord;
}
)";

static const char* shader_frag = R"(
#version 330 core

in vec3 worldFragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec3 light_color; //如果有好多光照呢
uniform vec3 object_color;

const float ambientStrength = 0.1;
const float diffuseStrength = 0.5;
const float specularStrength = 0.5;

vec4 ambient() {
	vec3 ambient_color = light_color * object_color * ambientStrength;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(vec3 light_direction, vec3 normal) {
	float diff = dot(-light_direction, normal); //漫反射分量, TODO: 把Normal的归一化放在顶点着色器做
	diff = max(diff, 0);
	vec3 diffu = vec3(diff * diffuseStrength * light_color);
	return vec4(diffu, 1.0);
}

vec4 specular(vec3 light_direction, vec3 normal, vec3 view_direction) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float reflect_light = dot(reflect_direction, view_direction);
	reflect_light = pow(max(reflect_light, 0.0), 32);
	vec3 reflect_color = reflect_light * specularStrength * light_color;
	return vec4(reflect_color, 1.0);
}

vec4 phone(vec3 light_direction, vec3 normal, vec3 view_direction) {
	return ambient() + diffuse(light_direction, normal) + specular(light_direction, normal, view_direction);
}

void main() {

	vec3 light_direction = normalize(worldFragPos - light_pos); //光线出射方向
	vec3 normal = normalize(Normal); //面法线向量
	vec3 view_direction = normalize(view_pos - worldFragPos); //片段位置指向观察者方向

	vec4 tex_color = mix(texture(tex, TexCoord), texture(tex2, TexCoord), 0.0);
	vec4 phone_color = phone(light_direction, normal, view_direction);
	FragColor = mix(tex_color, phone_color, 0.95);
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

