#pragma once

#include <functional>

namespace MuPDFWinRT
{
	public value struct RectF
	{
		float32 Left;
		float32 Top;
		float32 Right;
		float32 Bottom;
	};

	struct RectFEqual : public std::binary_function<const RectF, const RectF, bool> 
	{
		bool operator()( const RectF& _Left,  const RectF& _Right ) const
		{
			return (_Left.Left == _Right.Left) && (_Left.Top == _Right.Top) &&
				(_Left.Right == _Right.Right) && (_Left.Bottom == _Right.Bottom);
		};
	};
}