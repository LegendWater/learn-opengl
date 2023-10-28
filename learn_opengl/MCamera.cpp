#include "MCamera.h"

#include <gtc\type_ptr.hpp>
#include <Windows.h>

void MCamera::front_2_euler()
{
	pitch = glm::asin(front.y);
	yaw = glm::asin(front.x / glm::cos(pitch));
	roll = 0.0f;
}

void MCamera::euler_2_front()
{
	float sin_pitch = glm::sin(pitch), sin_yaw = glm::sin(yaw);
	float cos_pitch = glm::cos(pitch), cos_yaw = glm::cos(yaw);
	front.x = -cos_pitch * sin_yaw;
	front.y = sin_pitch;
	front.z = -cos_pitch * cos_yaw;
	front = glm::normalize(front); //确保长度为1
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
	updateView();
}

void MCamera::updateView()
{
	view = glm::lookAt(pos, pos + front, up);
}

void MCamera::updateProjection()
{
	projection = glm::perspective(fov, projection_rate, 0.01f, 100.0f);
}

MCamera::MCamera()
	:pos({ 0.0f, 0.0f, 0.0f }), front({ 0.0f, 0.0f,-1.0f }), up(world_up), fov(glm::radians(45.0f)), projection_rate(1.0f)
{
	right = glm::normalize(glm::cross(front, world_up));
	pitch = yaw = roll = 0.0f;
	updateView();
	updateProjection();
}

MCamera::MCamera(glm::vec3 pos, float fov, float whr)
	:pos(pos), up(world_up), fov(fov), projection_rate(whr)
{
	front = glm::normalize(pos != glm::vec3(0, 0, 0) ? -pos : glm::vec3(0, 0, -1));
	right = glm::normalize(glm::cross(front, world_up));
	
	roll = 0.0f;
	if (front.z == 0.0f) {
		yaw = front.x > 0 ? -pi / 2 : pi / 2;
		pitch = front.y > 0 ? pi / 2 : -pi / 2;
	}
	else
	{
		yaw = glm::atan(front.x / front.z);
		pitch = glm::atan(front.y / front.z);
	}
	//比较一下上下两种方法的结果
	front_2_euler();

	updateProjection();
}

MCamera::MCamera(const MCamera& cam)
	:pos(cam.pos), front(cam.front), up(cam.up), right(cam.right), view(cam.view),
	pitch(cam.pitch), yaw(cam.yaw), roll(cam.roll),
	fov(cam.fov), projection_rate(cam.projection_rate), projection(cam.projection)
{
}

MCamera::MCamera(MCamera&& cam) noexcept
	:pos(std::move(cam.pos)), front(std::move(cam.front)), up(std::move(cam.up)), right(std::move(cam.right)), view(std::move(cam.view)),
	pitch(std::move(cam.pitch)), yaw(std::move(cam.yaw)), roll(std::move(cam.roll)),
	fov(std::move(cam.fov)), projection_rate(std::move(cam.projection_rate)), projection(std::move(cam.projection))
{
}

void MCamera::shake(float radians_pitch, float radians_yaw)
{
	if (radians_pitch == 0 && radians_yaw == 0) {
		return;
	}
	//radians_pitch = fmodf(abs(radians_pitch), pi) - half_pi; //范围限制在[-pi/2, pi/2)
	pitch += radians_pitch;
	if (pitch <= -half_pi) pitch = -half_pi + 0.01f;
	else if (pitch >= half_pi) pitch = half_pi - 0.01f;

	//radians_yaw = fmod(abs(radians_yaw), _2pi) - pi; //范围限制在[-pi, pi)
	yaw += radians_yaw;
	yaw = fmod(yaw + pi, _2pi) - pi;
	char buf[1024]{ 0 };
	sprintf_s(buf, sizeof buf, "delta pitch=%.2f, delta yaw=%.2f, after shake: pitch=%.2f, yaw=%.2f\n", 
		radians_pitch, radians_yaw, pitch, yaw);
	OutputDebugStringA(buf);

	euler_2_front();
}

void MCamera::lookAt(glm::vec3 center)
{
	//如果look 正上方/正下方的话视角会锁住
	if (abs(center.x - pos.x) < 0.01f && abs(center.z - pos.z) < 0.01f) {
		center.x = pos.x + 0.01f;
		center.z = pos.z + 0.01f;
	}
	this->front = glm::normalize(center - pos);
	this->right = glm::normalize(glm::cross(front, world_up));
	this->up = glm::normalize(glm::cross(right, front));
	front_2_euler();
}

void MCamera::move(float front, float right, float up)
{
	pos += this->front * front;
	pos += this->right * right;
	pos += this->up * up;
	updateView();
}

void MCamera::moveTo(glm::vec3 pos)
{
	this->pos = pos;
	updateView();
}

void MCamera::zoom(float radians_zoom)
{
	if (radians_zoom == 0) {
		return;
	}
	fov += radians_zoom;
	if (fov < 0.01f) fov = 0.01f;
	else if (fov > glm::radians(89.0f)) fov = glm::radians(89.0f);

	char buf[1024]{ 0 };
	sprintf_s(buf, sizeof buf, "delta zoom=%.2f, after zoom fov=%.2f°\n", radians_zoom, glm::degrees(fov));
	OutputDebugStringA(buf);

	updateProjection();
}

void MCamera::zoomTo(float radians_zoom)
{
	if (fov == radians_zoom) {
		return;
	}
	fov = radians_zoom;
	if (fov < 0.01f) fov = 0.01f;
	else if (fov > glm::radians(89.0f)) fov = glm::radians(89.0f);

	char buf[1024]{ 0 };
	sprintf_s(buf, sizeof buf, "delta zoom=%.2f, after zoom fov=%.2f°\n", radians_zoom, glm::degrees(fov));
	OutputDebugStringA(buf);

	updateProjection();
}

glm::vec3 MCamera::getPos()
{
	return pos;
}

glm::vec3 MCamera::getFront()
{
	return front;
}

glm::vec3 MCamera::getUp()
{
	return up;
}

glm::vec3 MCamera::getRight()
{
	return right;
}

glm::vec3 MCamera::getEuler()
{
	return glm::vec3(pitch, yaw, roll);
}

glm::mat4 MCamera::getView()
{
	return view;
}

float MCamera::getFov()
{
	return fov;
}

glm::mat4 MCamera::getProjection()
{
	return projection;
}
