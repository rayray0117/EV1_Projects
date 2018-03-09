#pragma once
#include "Engine/Math/Matrix4.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////

class MatrixStack
{
public:
	MatrixStack(const Matrix4& startingTop = Matrix4());
	~MatrixStack(){};

	void make(const Matrix4& newMatrix);
	void transform(const Matrix4& transformMatrix);
	void pushThenTransform(const Matrix4& transformMatrix);
	void push();
	void pop();
	Matrix4 popGetLastValue();
	Matrix4 popGetNewValue();
	Matrix4 back();
	Matrix4 top();
private:
	std::vector<Matrix4> m_stack;
};