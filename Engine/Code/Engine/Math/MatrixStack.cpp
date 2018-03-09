#include "Engine/Math/MatrixStack.hpp"

MatrixStack::MatrixStack(const Matrix4& startingTop /*= Matrix4()*/)
{
	m_stack.push_back(startingTop);
}

//////////////////////////////////////////////////////////////////////////
void MatrixStack::make(const Matrix4& newMatrix)
{
	m_stack.back() = newMatrix;
}

void MatrixStack::transform(const Matrix4& transformMatrix)
{
	m_stack.back() = transformMatrix.getTransformed(back());
}

void MatrixStack::pushThenTransform(const Matrix4& transformMatrix)
{
	push();
	transform(transformMatrix);
}

void MatrixStack::push()
{
	m_stack.push_back(back());
}

void MatrixStack::pop()
{
	m_stack.pop_back();

	if (m_stack.empty())
	{
		m_stack.push_back(Matrix4());
	}
}

Matrix4 MatrixStack::popGetLastValue()
{
	Matrix4 result = back();
	pop();
	return result;
}

Matrix4 MatrixStack::popGetNewValue()
{
	pop();
	return back();
}

Matrix4 MatrixStack::back()
{
	return m_stack.back();
}

Matrix4 MatrixStack::top()
{
	return back();
}

