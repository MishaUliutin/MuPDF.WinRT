#pragma once

#include "MuPDFDoc.h"

namespace MuPDFWinRT
{
	public enum class DocumentType : int { PDF, XPS, CBZ };

	public value struct Point
	{
		int32 X;
		int32 Y;
	};

	public ref class Document sealed
	{    
	private:
		MuPDFDoc *m_doc;
		Windows::Storage::Streams::IBuffer^ m_buffer;
		Document();
		void Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType);
		unsigned char *GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer);
		const char *GetMIMEType(DocumentType documentType);
		void GotoPage(int page);
	public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType);
		virtual ~Document();
		Point GetPageSize(int pageNumber);

		property int PageCount
		{
			int get()
			{
				return m_doc->GetPageCount();
			}
		}
	};
}