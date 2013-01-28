// LinkInfo.cpp

#include "LinkInfo.h"

using namespace MuPDFWinRT;

LinkInfo::LinkInfo(float32 left, float32 top, float32 right, float32 bottom)
{
	m_rect.Left = left;
	m_rect.Top = top;
	m_rect.Right = right;
	m_rect.Bottom = bottom;
}