// Document.cpp
#include <Inspectable.h>
#include <client.h>
#include <Robuffer.h>
#include <memory>

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

void Document::Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType, int resolution)
{
	m_buffer = buffer;
	unsigned char *data = GetPointerToData(buffer);
	const char *type = GetMIMEType(documentType);
	Utilities::ThrowIfFailed(MuPDFDoc::Create(data, buffer->Length, type, resolution, &m_doc));
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

Point Document::GetPageSize(int pageNumber)
{
	Utilities::ThrowIfFailed(m_doc->GotoPage(pageNumber));
	Point size;
	size.X = m_doc->GetPageWidth();
	size.Y = m_doc->GetPageHeight();
	return size;
}
