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
	//��front�Ĳ��, yaw��Χ��[-pi, pi), pitch��Χ��(-pi/2, pi/2)��ʱ�뷽��Ϊ��, pitch��yaw�����(0, 0, -1), roll�����(0, 1, 0)Ŀǰ��Ϊ0
	float pitch, yaw, roll;

	float fov; //��ֱfov, ������
	float projection_rate; //ͶӰ����Ŀ�߱�

	glm::mat4 view;
	glm::mat4 projection;

	/**
	* ���ݸ��º��front����ŷ����
	*/
	void front_2_euler();
	/**
	* ���ݸ��º��ŷ���Ǹ���front��right��up��view����
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
	* radians_pitch > 0 ������ת, radians_yaw > 0 ������ת
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
