#pragma once

#include <memory>
#include <mutex>

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
		std::mutex m_lock; 
		MuPDFDoc *m_doc;
		Windows::Storage::Streams::IBuffer^ m_buffer;
		Document();
		OutlineItem^ CreateOutlineItem(std::shared_ptr<Outlineitem> item);
		ILinkInfo^ CreateLinkInfo(std::shared_ptr<MuPDFDocLink> link);
		void DrawPage(
			int32 pageNumber, 
			unsigned char *pixels, 
			int32 x, 
			int32 y, 
			int32 width, 
			int32 height,
			Platform::Boolean invert);
		void Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int resolution, unsigned int maxStore);
		void Init(Platform::String^ fileName, DocumentType documentType, int resolution, unsigned int maxStore);
		unsigned char *GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer);
		const char *GetMIMEType(DocumentType documentType);
	public:

        /// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="buffer">Buffer with document.</param>
        /// <param name="documentType">Document type.</param>
        /// <param name="resolution">Page bitmap dpi.</param>
        /// <returns>Returns new instance of the <see cref="Document"/>.</returns>
		[Windows::Foundation::Metadata::DefaultOverload]
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int32 resolution);

        /// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="fileName">File name.</param>
        /// <param name="documentType">Document type.</param>
        /// <param name="resolution">Page bitmap dpi.</param>
        /// <returns>Returns new instance of the <see cref="Document"/>.</returns>
		static Document^ Create(Platform::String^ fileName, DocumentType documentType, int32 resolution);

		/// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="buffer">Buffer with document.</param>
        /// <param name="documentType">Document type.</param>
        /// <param name="resolution">Page bitmap dpi.</param>
		/// <param name="maxStore">Maximum size in bytes of the resource store, before it will start evicting cached resources such as fonts and images. 0 (unlimited) can be used if a hard limit is not desired. Use default value (256MB) to get a reasonable size.</param>
        /// <returns>Returns new instance of the <see cref="Document"/>.</returns>
		[Windows::Foundation::Metadata::DefaultOverload]
		static Document^ Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int32 resolution, uint32 maxStore);

		/// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="fileName">File name.</param>
        /// <param name="documentType">Document type.</param>
        /// <param name="resolution">Page bitmap dpi.</param>
		/// <param name="maxStore">Maximum size in bytes of the resource store, before it will start evicting cached resources such as fonts and images. 0 (unlimited) can be used if a hard limit is not desired. Use default value (256MB) to get a reasonable size.</param>
        /// <returns>Returns new instance of the <see cref="Document"/>.</returns>
		static Document^ Create(Platform::String^ fileName, DocumentType documentType, int32 resolution, uint32 maxStore);
		virtual ~Document();
		Platform::Boolean AuthenticatePassword(Platform::String^ password);
		Point GetPageSize(int32 pageNumber);
		Windows::Foundation::Collections::IVector<ILinkInfo^>^ GetLinks(int32 pageNumber);	
		[Windows::Foundation::Metadata::DefaultOverload]
		void DrawPage(
			int32 pageNumber, 
			Platform::WriteOnlyArray<int>^ pixels, 
			int32 x, 
			int32 y, 
			int32 width, 
			int32 height,
			Platform::Boolean invert);
		void DrawPage(
			int32 pageNumber, 
			Windows::Storage::Streams::IBuffer^ pixels, 
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
				std::lock_guard<std::mutex> lock(m_lock);
				return m_doc->GetPageCount();
			}
		}
		property Platform::Boolean NeedsPassword
		{
			Platform::Boolean get()
			{
				std::lock_guard<std::mutex> lock(m_lock);
				return m_doc->NeedsPassword();
			}
		}
		property Platform::Boolean JavaSciptSupported
		{
			Platform::Boolean get()
			{
				std::lock_guard<std::mutex> lock(m_lock);
				return m_doc->JavaScriptSupported();
			}
		}
		property Platform::Boolean HasOutline
		{
			Platform::Boolean get()
			{
				std::lock_guard<std::mutex> lock(m_lock);
				return m_doc->HasOutline();
			}
		}
	};
}