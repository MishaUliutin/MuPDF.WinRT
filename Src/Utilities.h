#pragma once

#include <Winerror.h>

class Utilities
{
public:
	static void ThrowIfFailed(HRESULT hr);
};

