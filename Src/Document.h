#pragma once

#include <memory>

#include "OutlineItem.h"
#include "MuPDFDoc.h"
#include "LinkInfo.h"
#include "RectF.h"

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
		OutlineItem^ CreateOutlineItem(std::shared_ptr<Outlineitem> item);
		ILinkInfo^ CreateLinkInfo(std::shared_ptr<MuPDFDocLink> link);
		void Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int resolution);
		unsigned char *GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer);
		const char *GetMIMEType(DocumentType documentType);
	public:
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int32 resolution);
		virtual ~Document();
		Platform::Boolean AuthenticatePassword(Platform::String^ password);
		Point GetPageSize(int32 pageNumber);
		Windows::Foundation::Collections::IVector<ILinkInfo^>^ GetLinks(int32 pageNumber);	
		void DrawPage(
			int32 pageNumber, 
			Windows::Storage::Streams::IBuffer^ bitmap, 
			int32 x, 
			int32 y, 
			int32 width, 
			int32 height,
			Platform::Boolean invert);
		void UpdatePage(
			int32 pageNumber, 
			Windows::Storage::Streams::IBuffer^ bitmap, 
			int32 x, 
			int32 y, 
			int32 width, 
			int32 height,
			Platform::Boolean invert);
		Windows::Foundation::Collections::IVector<RectF>^ SearchText(int32 pageNumber, Platform::String^ text);
		Windows::Foundation::Collections::IVector<OutlineItem^>^ GetOutline();
		property int32 PageCount
		{
			int32 get()
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
		property Platform::Boolean HasOutline
		{
			Platform::Boolean get()
			{
				return m_doc->HasOutline();
			}
		}
	};
}