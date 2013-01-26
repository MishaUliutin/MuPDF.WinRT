#pragma once

extern "C" {
	#include "fitz.h"
	#include "mupdf.h"
}

#include <Winerror.h>

#define NUM_CACHE (3)

//TODO: Maybe I should change this to class/struct with default constructor
typedef struct
{
	int number;
	int width;
	int height;
	fz_rect mediaBox;
	fz_page *page;
	fz_page *hqPage;
	fz_display_list *pageList;
	fz_display_list *annotList;
} PageCache;

class MuPDFDoc
{
private:
	fz_context *m_context;
	fz_document *m_document;
	int m_currentPage;
	int m_resolution;
	PageCache m_pages[NUM_CACHE];
	MuPDFDoc(int resolution);
	HRESULT Init(unsigned char *buffer, int bufferLen, const char *mimeType);
	HRESULT InitContext();
	HRESULT InitDocument(unsigned char *buffer, int bufferLen, const char *mimeType);
	fz_stream *OpenStream(unsigned char *buffer, int bufferLen);
	void ClearPageCache(PageCache *pageCache);
	void ClearPages();
	int FindPageInCache(int pageNumber);
	int GetPageCacheIndex(int pageNumber);
//	void ClearHQPages();
public:
	static HRESULT Create(unsigned char *buffer, int bufferLen, const char *mimeType, int resolution, MuPDFDoc **obj);
	~MuPDFDoc(void);
	HRESULT GotoPage(int pageNumber);
	HRESULT DrawPage(unsigned char *bitmap, int x, int y, int width, int height, bool invert);
	inline int GetPageCount() { return fz_count_pages(m_document); }
	inline bool JavaScriptSupported() { return fz_javascript_supported() != 0; }
	inline bool NeedsPassword() { return fz_needs_password(m_document) != 0; }
	bool AuthenticatePassword(char *password);
	int GetPageWidth();
	int GetPageHeight();
};