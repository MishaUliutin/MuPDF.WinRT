#pragma once

extern "C" {
	#include "fitz.h"
	#include "mupdf.h"
}

namespace MuPDFWinRT
{
	public enum class DocumentType : int { PDF, XPS, CBZ };

	public ref class Document sealed
	{    
	private:
		fz_context *m_context;
		fz_document *m_document;
		Windows::Storage::Streams::IBuffer^ m_buffer;
		Document();
		void Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType);
		void InitContext();
		void InitDocument(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType);
		fz_stream *OpenStream(Windows::Storage::Streams::IBuffer^ buffer);
		unsigned char *GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer);
		const char *GetMIMEType(DocumentType documentType);
		void GotoPage(int page);
	public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType);
		virtual ~Document();

		property int PageCount
		{
			int get()
			{
				return fz_count_pages(m_document);
			}
		}
	};
}