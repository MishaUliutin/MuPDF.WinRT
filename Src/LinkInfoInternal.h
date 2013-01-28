#pragma once

#include "LinkInfo.h"

namespace MuPDFWinRT
{
	public ref class LinkInfoInternal sealed //: public LinkInfo 
	{
	private:
		int32 m_pageNumber;
	internal:
		LinkInfoInternal(float32 left, float32 top, float32 right, float32 bottom, int32 pageNumber);
		//void AcceptVisitor(MuPDFWinRT::LinkInfoVisitor^ visitor);
	public:
		property int32 PageNumber
		{
			int32 get()
			{
				return m_pageNumber;
			}
		}
	};
}