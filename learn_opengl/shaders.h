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
	Normal = mat3(transpose(inverse(model))) * aNormal; //TODO �ѷ��߾��󽻸�cpu����
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
	sampler2D diffuse; //sampler2D�ǲ�͸������, �����ó���uniform����ķ�ʽʵ����
	sampler2D specular;
	float shininess;
};

struct PointLight {
	vec3 pos;
	vec3 ambient; //�ѹ��յĻ�����ǿ��֮����������ø����ʻ᲻�����һ��?
	vec3 diffuse;
	vec3 specular;

	float constant; //���ߵĳ���˥��ϵ��
	float linear; //���ߵ�����˥��ϵ��
	float quadratic; //���ߵĶ���˥��ϵ��
};

uniform Material material;
#define POINT_LIGHT_NUM 4 //�ö��������
uniform PointLight light[POINT_LIGHT_NUM];

vec4 ambient(PointLight light, float attenuation) {
	vec3 ambient = vec3(texture(material.diffuse, TexCoord));
	vec3 ambient_color = light.ambient * ambient;
	ambient_color *= attenuation;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(PointLight light, vec3 light_direction, vec3 normal, float attenuation) {
	float diff = dot(-light_direction, normal); //������ǿ��
	diff = max(diff, 0);
	vec3 diffuse_color = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
	diffuse_color *= attenuation;
	
	return vec4(diffuse_color, 1.0);
}

vec4 specular(PointLight light, vec3 light_direction, vec3 normal, vec3 view_direction, float attenuation) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float spec = dot(reflect_direction, view_direction); //���淴��ǿ��
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

	vec3 normal = normalize(Normal); //�淨������, TODO: ��Normal�Ĺ�һ�����ڶ�����ɫ����
	vec4 phone_light = vec4(0.0); //���յĹ�����ɫ
	for (int i = 0; i < POINT_LIGHT_NUM; i++) {
		vec3 light_direction = normalize(worldFragPos - light[i].pos); //���߳��䷽��
		vec3 view_direction = normalize(view_pos - worldFragPos); //Ƭ��λ��ָ��۲��߷���
		float distance = length(worldFragPos - light[i].pos);
		float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //��Դ��Ƭ��λ�ö�˥��
		distance = length(worldFragPos - view_pos);
		attenuation *= 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //��Դ��۲���λ�ö�˥��

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
uniform vec3 pos; //��Դ����λ��
uniform vec3 view_pos;

uniform float constant; //���Դ�ĳ���˥��ϵ��
uniform float linear; //���Դ������˥��ϵ��
uniform float quadratic; //���Դ�Ķ���˥��ϵ��

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
	Normal = mat3(transpose(inverse(model))) * aNormal; //TODO �ѷ��߾��󽻸�cpu����
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
	sampler2D diffuse; //sampler2D�ǲ�͸������, �����ó���uniform����ķ�ʽʵ����
	bool diffuse_exist;
	sampler2D specular;
	bool specular_exist;
	float shininess;
};

struct PointLight {
	vec3 pos;
	vec3 ambient; //�ѹ��յĻ�����ǿ��֮����������ø����ʻ᲻�����һ��?
	vec3 diffuse;
	vec3 specular;

	float constant; //���ߵĳ���˥��ϵ��
	float linear; //���ߵ�����˥��ϵ��
	float quadratic; //���ߵĶ���˥��ϵ��
};

uniform Material material;

#define POINT_LIGHT_NUM 4 //�ö��������
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
	float diff = dot(-light_direction, normal); //������ǿ��
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
	float spec = dot(reflect_direction, view_direction); //���淴��ǿ��
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
	vec3 normal = normalize(Normal); //�淨������, TODO: ��Normal�Ĺ�һ�����ڶ�����ɫ����
	vec4 phone_light = vec4(0.0); //���յĹ�����ɫ
	for (int i = 0; i < POINT_LIGHT_NUM; i++) {
		vec3 light_direction = normalize(worldFragPos - light[i].pos); //���߳��䷽��
		vec3 view_direction = normalize(view_pos - worldFragPos); //Ƭ��λ��ָ��۲��߷���
		float distance = length(worldFragPos - light[i].pos);
		float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //��Դ��Ƭ��λ�ö�˥��
		distance = length(worldFragPos - view_pos);
		attenuation *= 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * pow(distance, 2)); //��Դ��۲���λ�ö�˥��

		phone_light += phone(light[i], light_direction, normal, view_direction, attenuation);
	}
	
	FragColor = phone_light;
}
)";

#endif // !SHADERS_H

