#pragma once

const double INVERSE_6 = 1.0 / 6.0;

template<typename S, typename F>
S RK4(double time, double deltaTime, const S& state, const F& funct)
{
	const double halfDT = 0.5*deltaTime;

	S state1		= state;
	S deltaState1	= funct(time, state1);
	S state2		= state + halfDT*deltaState1;
	S deltaState2	= funct(time + halfDT, state2);
	S state3		= state + halfDT*deltaState2;
	S deltaState3	= funct(time + halfDT, state3);
	S state4		= state + deltaTime*deltaState3;
	S result		= state + (deltaTime*INVERSE_6)*(deltaState1 + 2*deltaState2 + 2*deltaState3 + funct(time + deltaTime, state4));

	return result;
}