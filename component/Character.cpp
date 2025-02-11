#include "Character.h"
#include "../Base/BaseObject.h"
#include <iostream>
#include <math.h>
#include "../Maths/Vector.h"
#include "../Maths/MathUtil.h"
#include "../common/Macros.h"
#include "../vulkan/GlobalDeviceObjects.h"
#include "../vulkan/PhysicalDevice.h"
#include "../class/Timer.h"
#include "../class/UniformData.h"

DEFINITE_CLASS_RTTI(Character, BaseComponent);

std::shared_ptr<Character> Character::Create(const CharacterVariable& charVar)
{
	std::shared_ptr<Character> pChar = std::make_shared<Character>();
	if (pChar.get() && pChar->Init(pChar))
	{
		pChar->m_charVars = charVar;
		InputHub::GetInstance()->Register(pChar);
		return pChar;
	}
	return nullptr;
}

void Character::ProcessKey(KeyState keyState, uint8_t keyCode)
{
	switch (keyState)
	{
	case KEY_DOWN:
		switch (keyCode)
		{
		case 'W':
			m_moveFlag |= CharMoveDir::Forward;
			break;
		case 'S':
			m_moveFlag |= CharMoveDir::Backward;
			break;
		case 'A':
			m_moveFlag |= CharMoveDir::Leftward;
			break;
		case 'D':
			m_moveFlag |= CharMoveDir::Rightward;
			break;
		case 'Q':
			m_rotateFlag |= CharMoveDir::Leftward;
			break;
		case 'E':
			m_rotateFlag |= CharMoveDir::Rightward;
			break;
		}
		break;
	case KEY_UP:
		switch (keyCode)
		{
		case 'W':
			m_moveFlag &= ~(CharMoveDir::Forward);
			break;
		case 'S':
			m_moveFlag &= ~(CharMoveDir::Backward);
			break;
		case 'A':
			m_moveFlag &= ~(CharMoveDir::Leftward);
			break;
		case 'D':
			m_moveFlag &= ~(CharMoveDir::Rightward);
			break;
		case 'Q':
			m_rotateFlag &= ~(CharMoveDir::Leftward);
			break;
		case 'E':
			m_rotateFlag &= ~(CharMoveDir::Rightward);
			break;
		}
		break;
	}
}

void Character::ProcessMouse(KeyState keyState, const Vector2f& mousePosition)
{
	switch (keyState)
	{
	case KEY_DOWN:
		m_isControlInRotation = true;
		break;
	case KEY_UP:
		m_isControlInRotation = false;
		break;
	default:
		break;
	}
}

void Character::ProcessMouse(const Vector2f& mousePosition)
{
	m_lastSampleCursorPosition = mousePosition;
}

void Character::Move(const Vector3f& v, float delta)
{
	if (v == Vector3f(0, 0, 0))
		return;

	if (m_pObject.expired())
		return;

	Vector3f move_dir = v.Normal() * delta * m_charVars.moveSpeed;
	move_dir.x = -move_dir.x;	//reverse x axis, because camera left direction is opposite to x axis
	move_dir.z = -move_dir.z;	//reverse z axis, because camera looking direction is opposite to z axis

	std::shared_ptr<BaseObject> pObj = m_pObject.lock();
	Vector3f move_dir_local = pObj->GetLocalRotationM() * move_dir;
	pObj->SetPos(pObj->GetLocalPosition() + move_dir_local);
}

void Character::Move(uint32_t dir, float delta)
{
	if (dir == 0)
		return;

	Vector3f move_dir;
	if (dir & CharMoveDir::Forward)
		move_dir += Vector3f::Forward();
	if (dir & CharMoveDir::Backward)
		move_dir -= Vector3f::Forward();
	if (dir & CharMoveDir::Leftward)
		move_dir += Vector3f::Left();
	if (dir & CharMoveDir::Rightward)
		move_dir -= Vector3f::Left();
	Move(move_dir, delta);
}

void Character::OnRotateStart(const Vector2f& v)
{
	static float eps_cos = std::cosf(PI / 2.0f - EPSLON_ANGLE);	//minimum value of cosine minimum angle

	if (m_pObject.expired())
		return;

	m_rotationStarted = true;
	m_rotationStartPos = v;
	m_rotationStartMatrix = m_pObject.lock()->GetLocalRotationM();

	//get the angle between target direction and horizontal plane
	Vector3f target_dir = (Vector3f(0.0f, 0.0f, 0.0f) - m_rotationStartMatrix[2]).Normal();		//target direction
	Vector3f target_dir_xzporj = target_dir; target_dir_xzporj.y = 0.0f; target_dir_xzporj.Normalize();		//projection on xz plane
	float dot = target_dir * target_dir_xzporj;		//dot product of target direction and its projection on xz plane
	if (dot < eps_cos) dot = eps_cos;		//if the angle approaches to 90 degree, set it to a minimum value
	m_startTargetToH = std::acosf(dot);		//obtain the angle
	m_startTargetToH = target_dir.y > 0.0f ? m_startTargetToH : -m_startTargetToH;	//mark its sign
}

void Character::OnRotate(const Vector2f& v, bool started)
{
	if (started)
	{
		if (!m_rotationStarted)
			OnRotateStart(v);
		else
			Rotate(m_rotationStartPos - v);
	}
	else
	{
		if (m_rotationStarted)
			OnRotateEnd(v);
	}
}

void Character::OnRotateEnd(const Vector2f& v)
{
	Rotate(m_rotationStartPos - v);
	m_rotationStarted = false;
	m_rotationStartPos = Vector2f();
	m_rotationStartMatrix = Matrix3f();
	m_startTargetToH = 0.0f;
}

void Character::OnRotate(uint32_t dir, float delta)
{
	if (m_pObject.expired())
		return;

	if (dir == 0)
		return;

	Matrix3f rotate_around_up;

	if (dir & CharMoveDir::Leftward)
		rotate_around_up = Matrix3f::Rotation(delta * m_charVars.rotateSpeed, Vector3f::Upward());		//FIXME: Need profiler to replace hard-code delta
	if (dir & CharMoveDir::Rightward)
		rotate_around_up = Matrix3f::Rotation(-delta * m_charVars.rotateSpeed, Vector3f::Upward());		//FIXME: Need profiler to replace hard-code delta

	m_pObject.lock()->SetRotation(rotate_around_up * m_pObject.lock()->GetLocalRotationM());
}

void Character::Rotate(const Vector2f& v)
{
	if (m_pObject.expired())
		return;

	//get euler angle
	Vector2f euler_angle;
	euler_angle.x = -v.y * UniformData::GetInstance()->GetGlobalUniforms()->GetMainCameraVerticalFOV();
	euler_angle.y = v.x * UniformData::GetInstance()->GetGlobalUniforms()->GetMainCameraHorizontalFOV();		//reverse y direction, because of camera looks negative z axis

	//do this to prevent from over rotating
	//your neck can't rotate more than 90 degree around your shoulder, right?
	float current_target_to_H = m_startTargetToH + euler_angle.x;
	if (current_target_to_H > PI / 2.0f)
		euler_angle.x = PI / 2.0f - m_startTargetToH - EPSLON_ANGLE;
	else if (current_target_to_H < -PI / 2.0f)
		euler_angle.x = -PI / 2.0f - m_startTargetToH + EPSLON_ANGLE;

	//get local rotation matrix and its rotated x & y axis
	Vector3f obj_axis_x = m_rotationStartMatrix[0];
	Vector3f obj_axis_y = Vector3f::Upward();	//we use up as y axis, so that x axis would always keep horizontal

	Matrix3f rotate_around_x = Matrix3f::Rotation(euler_angle.x, obj_axis_x);
	Matrix3f rotate_around_y = Matrix3f::Rotation(euler_angle.y, obj_axis_y);

	//here we first rotate around axis x, then y
	//or we can't guarentee x to keep horizontal
	m_pObject.lock()->SetRotation(rotate_around_y * rotate_around_x * m_rotationStartMatrix);
}

void Character::Update()
{
	OnRotate({ m_lastSampleCursorPosition.x / GetPhysicalDevice()->GetSurfaceCap().currentExtent.width, (GetPhysicalDevice()->GetSurfaceCap().currentExtent.height - m_lastSampleCursorPosition.y) / GetPhysicalDevice()->GetSurfaceCap().currentExtent.height }, m_isControlInRotation);

	Move(m_moveFlag, Timer::GetElapsedTime());
	OnRotate(m_rotateFlag, Timer::GetElapsedTime());
}
