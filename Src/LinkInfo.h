#pragma once

#include "Document.h"
#include "LinkInfoVisitor.h"

namespace MuPDFWinRT
{
	private ref class LinkInfo
	{
	private:
		MuPDFWinRT::RectF m_rect;
	internal:
		LinkInfo(float32 left, float32 top, float32 right, float32 bottom);
		//virtual void AcceptVisitor(MuPDFWinRT::LinkInfoVisitor^ visitor) = 0;
	public:
		property MuPDFWinRT::RectF Rect
		{
			MuPDFWinRT::RectF get()
			{
				return m_rect;
			}
		}
	};
}