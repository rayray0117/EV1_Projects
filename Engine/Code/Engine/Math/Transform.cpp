#include "Engine/Math/Transform.hpp"
#include "../Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
Matrix4 SQT::getAsMatrix() const
{
	//PROFILE_SCOPE_FUNCTION();
	Matrix4 m; //Matrix4::CreateScale(scale);
	m.Scale(scale);
	Matrix4 rotM = rotation.getAsMatrix();
	//rotM.tranpose();
	m.concatenateTransform(rotM);
	//m.TransformDirection(position);
	m.setTranslation(position);
	return m;
}
/*/
Matrix4 SQT::getAsOtherMatrix() const
{
	Matrix4 m = Matrix4::CreateScale(scale);
	//m = rotation.getAsMatrix().getTransformed(m);
	m.concatenateTransform(rotation.getAsMatrix());
	m.concatenateTransform(Matrix4::CreateTranslation(position));
	//m.Scale(scale);
	return m;
}
//*/
void SQT::Combine(const SQT& transform)
{
	scale *= transform.scale;
	position += transform.position;
	rotation *= transform.rotation;
}

SQT SQT::getCombined(const SQT& transform) const
{
	SQT result;
	/*/
	result.scale = scale * transform.scale;
	result.position = position + transform.position;
	result.rotation = rotation * transform.rotation;
	//*/
	Matrix4 mine = this->getAsMatrix();
	Matrix4 trans = transform.getAsMatrix();
	mine.concatenateTransform(trans);
	result.setFromMatrix(mine);
	//*/
	return result;
}

void SQT::setFromMatrix(const Matrix4& inMatrix)
{
	//PROFILE_SCOPE_FUNCTION();
	position = inMatrix.getTranslation();
	rotation = Quaternion(inMatrix);
	scale = inMatrix.getScale();
	//*/
	if (scale.isMostlyEqualTo(Vector3::ONE))
		scale = Vector3::ONE; //Avoid floating point errors with scale!
	//*/
}

SQT SQT::getInverse() const
{
	Matrix4 inverseMat(getAsMatrix().getInverse());
	SQT inverseTransform;
	inverseTransform.setFromMatrix(inverseMat);
	return inverseTransform;
}

const SQT interpolate(const SQT& start, const SQT& end, float fractionToEnd)
{
	SQT result;

	result.position = interpolate(start.position, end.position, fractionToEnd);
	result.rotation = interpolate(start.rotation, end.rotation, fractionToEnd);
	result.scale = interpolate(start.scale, end.scale, fractionToEnd);

	return result;
}