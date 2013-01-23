// Document.cpp
#include <Inspectable.h>
#include <client.h>
#include <Robuffer.h>
#include "Document.h"
#include "Utilities.h"

using namespace MuPDFWinRT;
using namespace Platform;

Document::Document()
	: m_context(nullptr), m_document(nullptr), m_buffer(nullptr)
{
}

Document::~Document()
{
	if (m_document)
	{
		fz_close_document(m_document);
		m_document = nullptr;
	}
	if (m_buffer)
	{
		m_buffer = nullptr;
	}
	if (m_context)
	{
		fz_free_context(m_context);
		m_context = nullptr;
	}
}

void Document::Init(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType)
{
	InitContext();
	InitDocument(buffer, documentType);
}

void Document::InitContext()
{
	m_context = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!m_context)
	{
		throw ref new Platform::OutOfMemoryException();
	}
}

void Document::InitDocument(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType)
{
	m_buffer = buffer;
	fz_stream *stream = OpenStream(buffer);
	const char *type = GetMIMEType(documentType);
	fz_try(m_context)
	{
		m_document = fz_open_document_with_stream(m_context, type, stream);
	}
	fz_catch(m_context)
	{
		throw ref new Platform::InvalidArgumentException(ref new Platform::String(L"buffer"));
	}
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

fz_stream *Document::OpenStream(Windows::Storage::Streams::IBuffer^ buffer)
{
	unsigned char *data = GetPointerToData(buffer);
	fz_stream *stream = nullptr;
	fz_try(m_context)
	{
		stream = fz_open_memory(m_context, data, buffer->Length);
	}
	fz_catch(m_context)
	{
		throw ref new Platform::OutOfMemoryException(ref new Platform::String(L"fz_open_memory"));
	}
	return stream;
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
    unsigned char *pixels = nullptr;
    Utilities::ThrowIfFailed(bufferByteAccess->Buffer(&pixels));
    return pixels;
}

Document^ Document::Create(Windows::Storage::Streams::IBuffer^ buffer, DocumentType documentType)
{
	if (!buffer)
	{
		throw ref new Platform::InvalidArgumentException(L"buffer");
	}
	Document^ document = ref new Document();
	document->Init(buffer, documentType);
	return document;
}

