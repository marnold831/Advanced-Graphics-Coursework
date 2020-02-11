/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"

struct CameraNode {
	CameraNode(Vector3 pos) { position = pos; }
	Vector3 position;
};

class Camera	{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
		speed	= 0.5f;
		firstReturnDolly = false;
		rotate = false;
	};

	Camera(float pitch, float yaw, Vector3 position, Vector3 focus){
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
		speed = 0.01f;
		firstReturnDolly = false;
		rotate = false;
		focusPoint = focus;
	}

	~Camera(void){};

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	
	void SetRotate(bool rot) { rotate = rot; }

protected:

	void UpdatePosition();
	float distance(Vector3 a, Vector3 b)
	{
		return sqrtf((powf((b.x - a.x), 2) + powf((b.y - a.y), 2) + powf((b.z - a.z), 2)));
	}


	float	yaw;
	float	pitch;
	float	speed;

	Vector3 position;
	Vector3 focusPoint;
	
	bool rotate;
	bool firstReturnDolly;
	
};