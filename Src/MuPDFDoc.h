#pragma once

#include <memory>
#include <functional>
#include <vector>

#include <Winerror.h>

extern "C" {
	#include "fitz.h"
	#include "mupdf.h"
}


#define NUM_CACHE (3)
#define MAX_SEARCH_HITS (500)

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

typedef struct
{
	int level;
	//UTF-8
	std::unique_ptr<char[]> title;
	int pageNumber;
} Outlineitem;

template<class T>
struct Rect
{
    Rect() : left(), top(), right(), bottom() {}
    Rect(T left, T top, T right, T bottom) : 
        left(left), top(top), right(right), bottom(bottom) {}

    T left;
    T top;
    T right;
    T bottom;
};

typedef Rect<float> RectFloat;

/* Possible values of link type */
enum MuPDFDocLinkType { INTERNAL, URI, REMOTE };

typedef struct
{
	MuPDFDocLinkType type;
	float left; 
	float top;
	float right; 
	float bottom;
	// URI
	std::unique_ptr<char[]> uri;
	// INTERNAL
	int internalPageNumber;
	// REMOTE
	std::unique_ptr<char[]> fileSpec;
	int remotePageNumber;
	bool newWindow;	
} MuPDFDocLink;

class MuPDFDoc
{
private:
	fz_context *m_context;
	fz_document *m_document;
	fz_outline *m_outline;
	int m_currentPage;
	int m_resolution;
	PageCache m_pages[NUM_CACHE];
	MuPDFDoc(int resolution);
	//MuPDFDoc(const MuPDFDoc& that) = delete;
	HRESULT Init(unsigned char *buffer, int bufferLen, const char *mimeType);
	HRESULT InitContext();
	HRESULT InitDocument(unsigned char *buffer, int bufferLen, const char *mimeType);
	fz_stream *OpenStream(unsigned char *buffer, int bufferLen);
	void ClearPageCache(PageCache *pageCache);
	void ClearPages();
	int FindPageInCache(int pageNumber);
	int GetPageCacheIndex(int pageNumber);
	fz_matrix CalcConvertMatrix();
	int FillOutline(std::shared_ptr<std::vector<std::shared_ptr<Outlineitem>>> items, int position, fz_outline *outline, int level);
public:
	static HRESULT Create(unsigned char *buffer, int bufferLen, const char *mimeType, int resolution, MuPDFDoc **obj);
	~MuPDFDoc(void);
	HRESULT GotoPage(int pageNumber);
	HRESULT DrawPage(unsigned char *bitmap, int x, int y, int width, int height, bool invert);
	HRESULT UpdatePage(int pageNumber, unsigned char *bitmap, int x, int y, int width, int height, bool invert);
	bool AuthenticatePassword(char *password);
	inline int GetPageCount() { return fz_count_pages(m_document); }
	inline bool JavaScriptSupported() { return fz_javascript_supported() != 0; }
	inline bool NeedsPassword() { return fz_needs_password(m_document) != 0; }
	inline bool HasOutline() { return m_outline != nullptr; }
	int GetPageWidth();
	int GetPageHeight();
	std::shared_ptr<std::vector<std::shared_ptr<MuPDFDocLink>>> GetLinks();
	std::shared_ptr<std::vector<std::shared_ptr<RectFloat>>> SearchText(const char* text);
	std::shared_ptr<std::vector<std::shared_ptr<Outlineitem>>> GetOutline();
};