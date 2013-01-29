#include <memory>
#include <Windows.h>

#include "Utilities.h"

Platform::String^ Utilities::ConvertUTF8ToString(char* str)
{
	int length = MultiByteToWideChar(
		CP_UTF8, 
		0, 
		str, 
		-1, 
		nullptr, 
		0);
	if (length == 0)
		throw ref new Platform::FailureException();
	std::unique_ptr<wchar_t[]> convertedStr(new wchar_t[length]);
	length =  MultiByteToWideChar(
		CP_UTF8, 
		0, 
		str, 
		-1, 
		convertedStr.get(),
		length);
	if (length == 0)
		throw ref new Platform::FailureException();
	return ref new Platform::String(convertedStr.get());
}