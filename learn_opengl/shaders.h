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

//uniform sampler2D tex;
//uniform sampler2D tex2;

uniform vec3 view_pos;

struct Material {
	//vec3 ambient; ˵ɾ��ɾ��, ֻ����һ��
	//vec3 diffuse;
	sampler2D diffuse; //sampler2D�ǲ�͸������, �����ó���uniform����ķ�ʽʵ����
	//vec3 specular;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 pos;
	vec3 ambient; //�ѹ��յĻ�����ǿ��֮����������ø����ʻ᲻�����һ��?
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light; //����кö������

vec4 ambient() {
	vec3 ambient = vec3(texture(material.diffuse, TexCoord));
	vec3 ambient_color = light.ambient * ambient;
	return vec4(ambient_color, 1.0);
}

vec4 diffuse(vec3 light_direction, vec3 normal) {
	float diff = dot(-light_direction, normal); //������ǿ��, TODO: ��Normal�Ĺ�һ�����ڶ�����ɫ����
	diff = max(diff, 0);
	vec3 diffuse_color = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));
	
	return vec4(diffuse_color, 1.0);
}

vec4 specular(vec3 light_direction, vec3 normal, vec3 view_direction) {
	vec3 reflect_direction = reflect(light_direction, normal);
	float spec = dot(reflect_direction, view_direction); //���淴��ǿ��
	spec = pow(max(spec, 0.0), material.shininess);
	vec3 reflect_color = spec * light.specular * vec3(texture(material.specular, TexCoord));
	return vec4(reflect_color, 1.0);
}

vec4 phone(vec3 light_direction, vec3 normal, vec3 view_direction) {
	return ambient() + diffuse(light_direction, normal) + specular(light_direction, normal, view_direction);
}

void main() {

	vec3 light_direction = normalize(worldFragPos - light.pos); //���߳��䷽��
	vec3 normal = normalize(Normal); //�淨������
	vec3 view_direction = normalize(view_pos - worldFragPos); //Ƭ��λ��ָ��۲��߷���

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

