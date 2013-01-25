#include <math.h>

#include "MuPDFDoc.h"


MuPDFDoc::MuPDFDoc()
	: m_context(nullptr), m_document(nullptr), m_resolution(160), m_currentPage(-1)
{
	for(int i = 0; i < NUM_CACHE; i++)
	{
		m_pages[i].number = -1;
		m_pages[i].width = 0;
		m_pages[i].height = 0;
		m_pages[i].page = nullptr;
		m_pages[i].hqPage = nullptr;
		m_pages[i].pageList = nullptr;
		m_pages[i].annotList = nullptr;
	}
}

HRESULT MuPDFDoc::Create(unsigned char *buffer, int bufferLen, const char *mimeType, MuPDFDoc **obj)
{
	MuPDFDoc *doc = new MuPDFDoc();
	HRESULT result = doc->Init(buffer, bufferLen, mimeType);
	if (FAILED(result))
	{
		delete doc;
		return result;
	}
	else
	{
		*obj = doc;
		return S_OK;
	}
}

MuPDFDoc::~MuPDFDoc()
{
	if (m_document)
	{
		ClearPages();
		fz_close_document(m_document);
		m_document = nullptr;
	}
	if (m_context)
	{
		fz_free_context(m_context);
		m_context = nullptr;
	}
}

HRESULT MuPDFDoc::Init(unsigned char *buffer, int bufferLen, const char *mimeType)
{
	HRESULT result = InitContext();
	if (FAILED(result))
	{
		return result;
	}
	else
	{
		result = InitDocument(buffer, bufferLen, mimeType);
		return result;
	}
}

HRESULT MuPDFDoc::InitContext()
{
	m_context = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!m_context)
	{
		return E_OUTOFMEMORY;
	}
	else
	{
		return S_OK;
	}
}

HRESULT MuPDFDoc::InitDocument(unsigned char *buffer, int bufferLen, const char *mimeType)
{
	fz_stream *stream = OpenStream(buffer, bufferLen);
	if (!stream)
	{
		return E_OUTOFMEMORY;
	}
	else
	{
		fz_try(m_context)
		{
			m_document = fz_open_document_with_stream(m_context, mimeType, stream);
			//TODO: alerts_init(glo);
		}
		fz_catch(m_context)
		{
			return E_INVALIDARG;
		}
		return S_OK;
	}
}

fz_stream *MuPDFDoc::OpenStream(unsigned char *buffer, int bufferLen)
{
	fz_stream *stream = nullptr;
	fz_try(m_context)
	{
		stream = fz_open_memory(m_context, buffer, bufferLen);
	}
	fz_catch(m_context)
	{
		return nullptr;
	}
	return stream;
}

void MuPDFDoc::ClearPages()
{
	for(int i = 0; i < NUM_CACHE; i++)
	{
		ClearPageCache(&m_pages[i]);
	}
}

void MuPDFDoc::ClearPageCache(PageCache *pageCache)
{
	fz_free_display_list(m_context, pageCache->pageList);
	pageCache->pageList = NULL;
	fz_free_display_list(m_context, pageCache->annotList);
	pageCache->annotList = NULL;
	fz_free_page(m_document, pageCache->page);
	pageCache->page = NULL;
	fz_free_page(m_document, pageCache->hqPage);
	pageCache->hqPage = NULL;
}

int MuPDFDoc::FindPageInCache(int pageNumber)
{
	for(int i = 0; i < NUM_CACHE; i++)
	{
		if (m_pages[i].page != NULL && m_pages[i].number == pageNumber)
		{
			return i;
		}
	}
	return -1;
}

int MuPDFDoc::GetPageCacheIndex(int pageNumber)
{
	int furthest = 0;
	int furthestDist = -1;
	for (int i = 0; i < NUM_CACHE; i++)
	{
		if (m_pages[i].page == NULL)
		{
			/* cache record unused, and so a good one to use */
			return i;
		}
		else
		{
			int dist = abs(m_pages[i].number - pageNumber);
			/* Further away - less likely to be needed again */
			if (dist > furthestDist)
			{
				furthestDist = dist;
				furthest = i;
			}
		}
	}
	return furthest;
}

HRESULT MuPDFDoc::GotoPage(int pageNumber)
{
	int index = FindPageInCache(pageNumber);
	if (index >= 0)
	{
		m_currentPage = index;
		return S_OK;
	}

	index = GetPageCacheIndex(pageNumber);
	m_currentPage = index;
	PageCache *pageCache = &m_pages[m_currentPage];
	ClearPageCache(pageCache);
	/* In the event of an error, ensure we give a non-empty page */
	pageCache->width = 100;
	pageCache->height = 100;
	pageCache->number = pageNumber;

	fz_try(m_context)
	{
		pageCache->page = fz_load_page(m_document, pageCache->number);
		pageCache->mediaBox = fz_bound_page(m_document, pageCache->page);
		// fz_bound_page determine the size of a page at 72 dpi.
		float zoom = m_resolution / 72;
		fz_matrix ctm = fz_scale(zoom, zoom);
		fz_bbox bbox = fz_round_rect(fz_transform_rect(ctm, pageCache->mediaBox));
		pageCache->width = bbox.x1-bbox.x0;
		pageCache->height = bbox.y1-bbox.y0;
	}
	fz_catch(m_context)
	{
		return E_FAIL;
	}
	return S_OK;
}

void MuPDFDoc::ClearHQPages()
{
	for (int i = 0; i < NUM_CACHE; i++) 
	{
		fz_free_page(m_document, m_pages[i].hqPage);
		m_pages[i].hqPage = NULL;
	}
}

HRESULT MuPDFDoc::DrawPage(unsigned char *bitmap, int pageW, int pageH, int patchX, int patchY, int patchW, int patchH)
{
	return E_NOTIMPL;
}
