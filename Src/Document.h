#pragma once

extern "C" {
#include "mupdf.h"
}

namespace MuPDFWinRT
{
    public ref class Document sealed
    {
	private:
		fz_context *m_context;
		Document();
		void Init(Windows::Storage::Streams::IBuffer^ buffer);
		void InitContext();
    public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer);
		virtual ~Document();
    };
}