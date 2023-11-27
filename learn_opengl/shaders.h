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


uniform vec3 view_pos;

struct Material {
	sampler2D diffuse; //sampler2D是不透明类型, 不能用除了uniform以外的方式实例化
	sampler2D specular;
	float shininess;
};

struct PointLight {
	vec3 pos;
	vec3 ambient; //把光照的环境光强度之类的属性设置给材质会不会更好一点?
	vec3 diffuse;
	vec3 specular;

	float constant; //光线的常量衰减系数
	float linear; //光线的线性衰减系数
	float quadratic; //光线的二次衰减系数
};

uniform Material material;
#define POINT_LIGHT_NUM 4 //好多光照来啦
uniform PointLight light[POINT_LIGHT_NUM];

vec4 ambient(PointLight light, float attenuation) {
	vec3 ambient = vec3(texture(material.diffuse, TexCoord));
	vec3 ambient_color = light.ambient * ambient;
	ambient_color *= attenuation;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(PointLight light, vec3 light_direction, vec3 normal, float attenuation) {
	float diff = dot(-light_direction, normal); //漫反射强度
	diff = max(diff, 0);
	vec3 diffuse_color = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
	diffuse_color *= attenuation;
	
	return vec4(diffuse_color, 1.0);
}

vec4 specular(PointLight light, vec3 light_direction, vec3 normal, vec3 view_direction, float attenuation) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float spec = dot(reflect_direction, view_direction); //镜面反光强度
	spec = pow(max(spec, 0.0), material.shininess);
	vec3 reflect_color = spec * light.specular * vec3(texture(material.specular, TexCoord));
	reflect_color *= attenuation;

	return vec4(reflect_color, 1.0);
}

vec4 phone(PointLight light, vec3 light_direction, vec3 normal, vec3 view_direction, float attenuation) {
	return ambient(light, attenuation) + 
			diffuse(light, light_direction, normal, attenuation) + 
			specular(light, light_direction, normal, view_direction, attenuation);
}

void main() {

	vec3 normal = normalize(Normal); //面法线向量, TODO: 把Normal的归一化放在顶点着色器做
	vec4 phone_light = vec4(0.0); //最终的光照颜色
	for (int i = 0; i < POINT_LIGHT_NUM; i++) {
		vec3 light_direction = normalize(worldFragPos - light[i].pos); //光线出射方向
		vec3 view_direction = normalize(view_pos - worldFragPos); //片段位置指向观察者方向
		float distance = length(worldFragPos - light[i].pos);
		float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //光源因片段位置而衰减
		distance = length(worldFragPos - view_pos);
		attenuation *= 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //光源因观察者位置而衰减

		phone_light += phone(light[i], light_direction, normal, view_direction, attenuation);
	}
	
	FragColor = phone_light;
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
uniform vec3 pos; //光源世界位置
uniform vec3 view_pos;

uniform float constant; //点光源的常量衰减系数
uniform float linear; //点光源的线性衰减系数
uniform float quadratic; //点光源的二次衰减系数

void main() {
	float distance = length(pos - view_pos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * pow(distance, 2));
	vec3 light_color = color * attenuation;

	FragColor = vec4(light_color, 1.0);
}
)";

static const char* shader_nanosuit_vert = R"(
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

static const char* shader_nanosuit_frag = R"(
#version 330 core

in vec3 worldFragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;


uniform vec3 view_pos;

struct Material {
	sampler2D diffuse; //sampler2D是不透明类型, 不能用除了uniform以外的方式实例化
	bool diffuse_exist;
	sampler2D specular;
	bool specular_exist;
	float shininess;
};

struct PointLight {
	vec3 pos;
	vec3 ambient; //把光照的环境光强度之类的属性设置给材质会不会更好一点?
	vec3 diffuse;
	vec3 specular;

	float constant; //光线的常量衰减系数
	float linear; //光线的线性衰减系数
	float quadratic; //光线的二次衰减系数
};

uniform Material material;

#define POINT_LIGHT_NUM 4 //好多光照来啦
uniform PointLight light[POINT_LIGHT_NUM];

vec4 ambient(PointLight light, float attenuation) {
	vec3 ambient;
	if (material.diffuse_exist) {
		ambient = vec3(texture(material.diffuse, TexCoord));
	}else {
		ambient = vec3(0.2);
	}
	vec3 ambient_color = light.ambient * ambient;
	ambient_color *= attenuation;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(PointLight light, vec3 light_direction, vec3 normal, float attenuation) {
	float diff = dot(-light_direction, normal); //漫反射强度
	diff = max(diff, 0);
	vec3 diffuse_color;
	if (material.diffuse_exist) {
		diffuse_color = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
	}else {
		diffuse_color = vec3(0.2);
	}
	diffuse_color *= attenuation;
	
	return vec4(diffuse_color, 1.0);
}

vec4 specular(PointLight light, vec3 light_direction, vec3 normal, vec3 view_direction, float attenuation) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float spec = dot(reflect_direction, view_direction); //镜面反光强度
	spec = pow(max(spec, 0.0), material.shininess);
	vec3 reflect_color;
	if (material.specular_exist) {
		reflect_color = spec * light.specular * vec3(texture(material.specular, TexCoord));
	}else {
		reflect_color = vec3(0.0);
	}
	reflect_color *= attenuation;

	return vec4(reflect_color, 1.0);
}

vec4 phone(PointLight light, vec3 light_direction, vec3 normal, vec3 view_direction, float attenuation) {
	return ambient(light, attenuation) + 
			diffuse(light, light_direction, normal, attenuation) + 
			specular(light, light_direction, normal, view_direction, attenuation);
}

void main() {
	vec3 normal = normalize(Normal); //面法线向量, TODO: 把Normal的归一化放在顶点着色器做
	vec4 phone_light = vec4(0.0); //最终的光照颜色
	for (int i = 0; i < POINT_LIGHT_NUM; i++) {
		vec3 light_direction = normalize(worldFragPos - light[i].pos); //光线出射方向
		vec3 view_direction = normalize(view_pos - worldFragPos); //片段位置指向观察者方向
		float distance = length(worldFragPos - light[i].pos);
		float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //光源因片段位置而衰减
		distance = length(worldFragPos - view_pos);
		attenuation *= 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //光源因观察者位置而衰减

		phone_light += phone(light[i], light_direction, normal, view_direction, attenuation);
	}
	
	FragColor = phone_light;
}
)";

#endif // !SHADERS_H

