#pragma once

#include <Winerror.h>

namespace Utilities
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw Platform::Exception::CreateException(hr);
		}
	}

	Platform::String^ ConvertUTF8ToString(char* str);
} 