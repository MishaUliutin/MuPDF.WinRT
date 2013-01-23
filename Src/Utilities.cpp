// Utilities.cpp
#include "Utilities.h"

using namespace Platform;

void Utilities::ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
}