#pragma once

enum class EEventCode
{
	C2S_Plus = 1,
	C2S_Minus = 2,
	C2S_Multiply = 3,
	C2S_Divide = 4,
	C2S_Remain = 5,
	S2C_Result = 6,
};


//CPP
struct Numbers
{
	short FirstNumber;
	short SecondNumbers;
};

struct ResultNumber
{
	int Number;
};