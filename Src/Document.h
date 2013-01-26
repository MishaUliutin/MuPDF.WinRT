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
		void Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int resolution);
		unsigned char *GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer);
		const char *GetMIMEType(DocumentType documentType);
		void GotoPage(int page);
	public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int32 resolution);
		virtual ~Document();
		Platform::Boolean AuthenticatePassword(Platform::String^ password);
		Point GetPageSize(int32 pageNumber);
		void DrawPage(
			int32 pageNumber, 
			Windows::Storage::Streams::IBuffer^ bitmap, 
			int32 x, 
			int32 y, 
			int32 width, 
			int32 height,
			Platform::Boolean invert);

		property int32 PageCount
		{
			int get()
			{
				return m_doc->GetPageCount();
			}
		}
		property Platform::Boolean NeedsPassword
		{
			Platform::Boolean get()
			{
				return m_doc->NeedsPassword();
			}
		}
		property Platform::Boolean JavaSciptSupported
		{
			Platform::Boolean get()
			{
				return m_doc->JavaScriptSupported();
			}
		}
	};
}