#pragma once
#ifndef MCAMERA_H
#define MCAMERA_H

#include <glm.hpp>

static glm::vec3 world_right(1.0f, 0.0f, 0.0f);
static glm::vec3 world_up(0.0f, 1.0f, 0.0f);
static glm::vec3 world_deep(0.0f, 0.0f, 1.0f);

static const float pi = 3.1415926f;
static const float _2pi = pi * 2.0f;
static const float half_pi = pi / 2.0f;

class MCamera
{
private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	//对front的拆解, yaw范围在[-pi, pi), pitch范围在(-pi/2, pi/2)逆时针方向为正, pitch和yaw相对于(0, 0, -1), roll相对于(0, 1, 0)目前恒为0
	float pitch, yaw, roll;

	float fov; //垂直fov, 弧度制
	float projection_rate; //投影矩阵的宽高比

	glm::mat4 view;
	glm::mat4 projection;

	/**
	* 根据更新后的front更新欧拉角
	*/
	void front_2_euler();
	/**
	* 根据更新后的欧拉角更新front、right、up和view矩阵
	*/
	void euler_2_front();

	void updateView();
	void updateProjection();
public:
	MCamera();
	MCamera(glm::vec3 pos, float fov, float whr);
	MCamera(const MCamera& cam);
	MCamera(MCamera&& cam) noexcept;

	/**
	* radians_pitch > 0 则往上转, radians_yaw > 0 则往左转
	*/
	void shake(float radians_pitch, float radians_yaw);
	void lookAt(glm::vec3 center);
	void move(float front, float right, float up);
	void moveTo(glm::vec3 pos);
	void zoom(float radians_zoom);
	void zoomTo(float radians_zoom);

	glm::vec3 getPos();
	glm::vec3 getFront();
	glm::vec3 getUp();
	glm::vec3 getRight();
	glm::vec3 getEuler();
	glm::mat4 getView();
	float getFov();
	glm::mat4 getProjection();
};
#endif // !MCAMERA_H
