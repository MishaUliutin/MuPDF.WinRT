// Document.cpp

#include <vector>
#include <memory>

#include <Inspectable.h>
#include <client.h>
#include <Robuffer.h>
#include <collection.h>

#include "Document.h"
#include "Utilities.h"

using namespace MuPDFWinRT;

Document::Document()
	: m_doc(nullptr), m_buffer(nullptr)
{
}

Document::~Document()
{
	if (m_doc)
	{
		delete m_doc;
		m_doc = nullptr;
	}
	if (m_buffer)
	{
		m_buffer = nullptr;
	}
}

Document^ Document::Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int32 resolution)
{
	if (!buffer)
	{
		throw ref new Platform::InvalidArgumentException(L"buffer");
	}
	Document^ document = ref new Document();
	document->Init(buffer, documentType, resolution);
	return document;
}

Platform::Boolean Document::AuthenticatePassword(Platform::String^ password)
{
	int ansiLength =  WideCharToMultiByte(
		CP_ACP, 
		0, 
		password->Data(), 
		-1, 
		nullptr, 
		0, 
		nullptr, 
		nullptr);
	if (ansiLength == 0)
		throw ref new Platform::FailureException();
	std::unique_ptr<char[]> ansiPassword(new char[ansiLength]);
	ansiLength =  WideCharToMultiByte(
		CP_ACP, 
		0, 
		password->Data(), 
		-1, 
		ansiPassword.get(),
		ansiLength,
		nullptr, 
		nullptr);
	if (ansiLength == 0)
		throw ref new Platform::FailureException();
	return m_doc->AuthenticatePassword(ansiPassword.get());
}

Point Document::GetPageSize(int pageNumber)
{
	Utilities::ThrowIfFailed(m_doc->GotoPage(pageNumber));
	Point size;
	size.X = m_doc->GetPageWidth();
	size.Y = m_doc->GetPageHeight();
	return size;
}

Windows::Foundation::Collections::IVector<ILinkInfo^>^ Document::GetLinks(int32 pageNumber)
{
	Utilities::ThrowIfFailed(m_doc->GotoPage(pageNumber));
	auto links = m_doc->GetLinks();
	auto items = ref new Platform::Collections::Vector<ILinkInfo^>();
	for(size_t i = 0; i < links->size(); i++)
	{
		auto linkInfo = CreateLinkInfo(links->at(i));
		items->InsertAt(i, linkInfo);
	}
	return items;
}

void Document::DrawPage(
	int32 pageNumber, 
	Windows::Storage::Streams::IBuffer^ bitmap, 
	int32 x, 
	int32 y, 
	int32 width, 
	int32 height,
	Platform::Boolean invert)
{
	Utilities::ThrowIfFailed(m_doc->GotoPage(pageNumber));
	auto buffer = GetPointerToData(bitmap);
	Utilities::ThrowIfFailed(m_doc->DrawPage(buffer, x, y, width, height, invert));
}

void Document::UpdatePage(
	int32 pageNumber, 
	Windows::Storage::Streams::IBuffer^ bitmap, 
	int32 x, 
	int32 y, 
	int32 width, 
	int32 height,
	Platform::Boolean invert)
{
	auto buffer = GetPointerToData(bitmap);
	Utilities::ThrowIfFailed(m_doc->UpdatePage(pageNumber, buffer, x, y, width, height, invert));
}

Windows::Foundation::Collections::IVector<RectF>^ Document::SearchText(int32 pageNumber, Platform::String^ text)
{
	Utilities::ThrowIfFailed(m_doc->GotoPage(pageNumber));
	auto ut8Text = Utilities::ConvertStringToUTF8(text);
	auto hints = m_doc->SearchText(ut8Text.get());
	if (!hints)
		throw ref new Platform::OutOfMemoryException();
	auto items = ref new Platform::Collections::Vector<RectF, RectFEqual>();
	for(size_t i = 0; i < hints->size(); i++)
	{
		auto hint = hints->at(i);
		RectF rect = Utilities::CreateRectF(hint->left, hint->top, hint->right, hint->bottom);
		items->InsertAt(i, rect);
	}
	return items;
}

Windows::Foundation::Collections::IVector<OutlineItem^>^ Document::GetOutline()
{
	auto items = m_doc->GetOutline();
	auto outlineItems = ref new Platform::Collections::Vector<MuPDFWinRT::OutlineItem^>();
	for(size_t i = 0; i < items->size(); i++)
	{
		auto outlineItem = CreateOutlineItem(items->at(i));
		outlineItems->InsertAt(i, outlineItem);
	}
	return outlineItems;
}

MuPDFWinRT::OutlineItem^ Document::CreateOutlineItem(std::shared_ptr<Outlineitem> item)
{
	auto title = Utilities::ConvertUTF8ToString(item->title.get());
	return ref new MuPDFWinRT::OutlineItem(item->pageNumber, item->level, title);
}

ILinkInfo^ Document::CreateLinkInfo(std::shared_ptr<MuPDFDocLink> link)
{
	ILinkInfo^ linkInfo;
	RectF rect = Utilities::CreateRectF(link->left, link->top, link->right, link->bottom);
	switch (link->type)
	{
	case INTERNAL:
		{
			linkInfo = ref new LinkInfoInternal(rect, link->internalPageNumber);
			break;
		}
	case URI:
		{
			auto uri = Utilities::ConvertUTF8ToString(link->uri.get());
			linkInfo = ref new LinkInfoURI(rect, uri);
			break;
		}
	case REMOTE:
		{
			auto fileSpec = Utilities::ConvertUTF8ToString(link->fileSpec.get());
			linkInfo = ref new LinkInfoRemote(rect, fileSpec, link->remotePageNumber, link->newWindow);
			break;
		}
	};
	return linkInfo;
}

void Document::Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int resolution)
{
	m_buffer = buffer;
	unsigned char *data = GetPointerToData(buffer);
	const char *type = GetMIMEType(documentType);
	Utilities::ThrowIfFailed(MuPDFDoc::Create(data, buffer->Length, type, resolution, &m_doc));
}

unsigned char *Document::GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer)
{
	// Cast to Object^, then to its underlying IInspectable interface.
	Object^ obj = buffer;
	Microsoft::WRL::ComPtr<IInspectable> insp(reinterpret_cast<IInspectable*>(obj));

	// Query the IBufferByteAccess interface.
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
	Utilities::ThrowIfFailed(insp.As(&bufferByteAccess));

	// Retrieve the buffer data.
	unsigned char *data = nullptr;
	Utilities::ThrowIfFailed(bufferByteAccess->Buffer(&data));
	return data;
}

const char *Document::GetMIMEType(DocumentType documentType)
{
	switch (documentType)
	{
	case DocumentType::PDF:
		return "application/pdf";
	case DocumentType::XPS:
		return "application/vnd.ms-xpsdocument";
	case MuPDFWinRT::DocumentType::CBZ:
		return "application/x-cbz";
	default:
		return "application/pdf";
	}
}