#include"Camera.h"


void Camera::UpdateCameraVectors()
{
	
	GLfloat front_x = cos(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));//x
	GLfloat front_y = sin(glm::radians(this->m_Pitch)); //y
	GLfloat front_z = sin(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));//z

	glm::vec3 front(front_x, front_y, front_z);

	this->m_Front = glm::normalize(front);
	this->m_Right = glm::normalize(glm::cross(this->m_Front, m_WorldUp));
	this->m_Up = glm::normalize(glm::cross(this->m_Right, this->m_Front));
}


glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->m_Position, this->m_Position + this->m_Front, this->m_Up);
}

GLfloat Camera::GetZoom()
{
	return this->m_Zoom;
}


glm::vec3  Camera::GetCameraPosition(void)
{
	return this->m_Position;
}


glm::vec3 Camera::GetCameraFront(void)
{
	return this->m_Front;
}

glm::vec3 Camera::GetCameraUp(void)
{
	return this->m_Up;
}

glm::vec3 Camera::GetCameraRight(void) 
{
	return this->m_Right;
}


void Camera::SetCameraPosition(glm::vec3 newPosition)
{
	this->m_Position = newPosition;
}

void Camera::SetCameraFront(glm::vec3 newFront)
{
	this->m_Front = newFront;
}


void Camera::InvertCamera()
{
	GLfloat currentX = this->m_Position.x;
	GLfloat currentY = this->m_Position.y;
	GLfloat currentZ = this->m_Position.z;

	GLfloat currentPitch = this->m_Pitch;

	// invert both camera and pitch
	this->m_Position = glm::vec3(currentX, -1.0f * currentY, currentZ);
	this->m_Pitch = -1.0f* currentPitch;

	// UpdateCameraVectors() must be called after every pitch change
	this->UpdateCameraVectors();
}


//void Camera::InvertPitch()
//{
//	this->m_Pitch = -this->m_Pitch;
//	UpdateCameraVectors();
//}


void Camera::ProcessKeyBoard(ECameraMovement direction,GLfloat deltaTime)
{
	GLfloat velocity = this->m_MovementSpeed;// *deltaTime;
		

	switch (direction)
	{
		case E_FORWARD:
			this->m_Position += this->m_Front * velocity;
			break;
		
		case E_BACKARD:
			this->m_Position -= this->m_Front * velocity;
			break;

		case E_LEFT:
			this->m_Position -= this->m_Right * velocity;
			break;

		case E_RIGHT:
			this->m_Position += this->m_Right * velocity;
			break;

		case E_UP:
			this->m_Position += this->m_Up * velocity;
			break;

		case E_DOWN:
			this->m_Position -= this->m_Up * velocity;
			break;
		default:
			break;
	}
}

void Camera::ProcessMouseMovements(GLfloat xOffset, GLfloat yOffset,GLboolean constarintPitch)
{
	xOffset *= this->m_MouseSensitivity;
	yOffset *= this->m_MouseSensitivity;

	this->m_Yaw = this->m_Yaw + xOffset;
	this->m_Pitch = this->m_Pitch + yOffset;

	if (constarintPitch)
	{
		if (this->m_Pitch > 360.0f)
		{
			this->m_Pitch = 360.0f;
		}

		if (this->m_Pitch < -360.0f)
		{
			this->m_Pitch = -360.0f;
		}
	}

	/*if (constarintPitch)
	{
		if (this->m_Pitch > 89.0f)
		{
			this->m_Pitch = 89.0f;
		}

		if (this->m_Pitch < -89.0f)
		{
			this->m_Pitch = -89.0f;
		}
	}*/
	
	this->UpdateCameraVectors();
}


void Camera::ProcessMouseScrool(GLfloat yOffset, GLfloat xOffset)
{
	if (this->m_Zoom >= 1.0f && this->m_Zoom <= 45.0f)
	{
		this->m_Zoom -= yOffset;
	}

	if (this->m_Zoom <= 1.0f)
	{
		this->m_Zoom = 1.0f;
	}

	if (this->m_Zoom >= 45.0f)
	{
		this->m_Zoom = 45.0f;
	}

}


void Camera::MoveForward(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position += this->m_Front * Velocity;
}

void Camera::MoveBackward(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position -= this->m_Front * Velocity;
}

void Camera::MoveUpward(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position += this->m_Up * Velocity;
}

void Camera::MoveDownward(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position -= this->m_Up * Velocity;
}

void Camera::StrafeRight(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position += this->m_Right*Velocity;
}

void Camera::StrafeLeft(GLfloat Velocity, GLfloat DeltaTime)
{
	this->m_Position -= this->m_Right*Velocity;
}

GLfloat Camera::GetYaw()
{
	return this->m_Yaw;
}

GLfloat Camera::GetPitch()
{
	return this->m_Pitch;
}

GLfloat Camera::GetRoll()
{
	return this->m_Roll;
}

void Camera::SetYaw(GLfloat newYaw)
{
	this->m_Yaw = newYaw;
}

void Camera::SetPitch(GLfloat newPitch)
{
	this->m_Pitch = newPitch;
}

void Camera::SetRoll(GLfloat newRoll)
{
	this->m_Roll = newRoll;
}

