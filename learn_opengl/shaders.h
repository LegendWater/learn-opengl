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

//uniform sampler2D tex;
//uniform sampler2D tex2;

uniform vec3 view_pos;

struct Material {
	//vec3 ambient; 说删就删了, 只活了一集
	//vec3 diffuse;
	sampler2D diffuse; //sampler2D是不透明类型, 不能用除了uniform以外的方式实例化
	//vec3 specular;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 pos;
	vec3 ambient; //把光照的环境光强度之类的属性设置给材质会不会更好一点?
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light; //如果有好多光照呢

vec4 ambient() {
	vec3 ambient = vec3(texture(material.diffuse, TexCoord));
	vec3 ambient_color = light.ambient * ambient;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(vec3 light_direction, vec3 normal) {
	float diff = dot(-light_direction, normal); //漫反射强度, TODO: 把Normal的归一化放在顶点着色器做
	diff = max(diff, 0);
	vec3 diffuse_color = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
	
	return vec4(diffuse_color, 1.0);
}

vec4 specular(vec3 light_direction, vec3 normal, vec3 view_direction) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float spec = dot(reflect_direction, view_direction); //镜面反光强度
	spec = pow(max(spec, 0.0), material.shininess);
	vec3 reflect_color = spec * light.specular * vec3(texture(material.specular, TexCoord));
	return vec4(reflect_color, 1.0);
}

vec4 phone(vec3 light_direction, vec3 normal, vec3 view_direction) {
	return ambient() + diffuse(light_direction, normal) + specular(light_direction, normal, view_direction);
}

void main() {

	vec3 light_direction = normalize(worldFragPos - light.pos); //光线出射方向
	vec3 normal = normalize(Normal); //面法线向量
	vec3 view_direction = normalize(view_pos - worldFragPos); //片段位置指向观察者方向

	//vec4 tex_color = mix(texture(tex, TexCoord), texture(tex2, TexCoord), 0.0);
	vec4 phone_color = phone(light_direction, normal, view_direction);
	//FragColor = mix(tex_color, phone_color, 0.95);
	FragColor = phone_color;
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

