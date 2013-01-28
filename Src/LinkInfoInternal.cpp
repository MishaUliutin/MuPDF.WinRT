// LinkInfoInternal.cpp

#include "LinkInfoInternal.h"

using namespace MuPDFWinRT;

LinkInfoInternal::LinkInfoInternal(float32 left, float32 top, float32 right, float32 bottom, int32 pageNumber)
	//: LinkInfo(left, top, right, bottom)
{
	m_pageNumber = pageNumber;
}

//void LinkInfoInternal::AcceptVisitor(MuPDFWinRT::LinkInfoVisitor^ visitor)
//{
//
//}
