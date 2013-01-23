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
		fz_document *m_document;
		Document();
		void Init(Windows::Storage::Streams::IBuffer^ buffer);
		void InitContext();
		void InitDocument(Windows::Storage::Streams::IBuffer^ buffer);
    public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer);
		virtual ~Document();
    };
}