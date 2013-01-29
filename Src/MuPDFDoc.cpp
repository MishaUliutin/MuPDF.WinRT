#include <math.h>

#include "MuPDFDoc.h"

MuPDFDoc::MuPDFDoc(int resolution)
	: m_context(nullptr), m_document(nullptr), m_resolution(resolution), m_currentPage(-1)/*, m_alertEventCallback(nullptr)*/
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

HRESULT MuPDFDoc::Create(unsigned char *buffer, int bufferLen, const char *mimeType, int resolution, MuPDFDoc **obj)
{
	MuPDFDoc *doc = new MuPDFDoc(resolution);
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

//HRESULT MuPDFDoc::Create(unsigned char *buffer, int bufferLen, const char *mimeType, int resolution, AlertEventCallback alertEventCallback, MuPDFDoc **obj)
//{
//	HRESULT result = Create(buffer, bufferLen, mimeType, resolution, obj);
//	if (FAILED(result))
//	{
//		return result;
//	}
//	else
//	{
//		(*obj)->m_alertEventCallback = alertEventCallback;
//		return S_OK;
//	}
//}
//
MuPDFDoc::~MuPDFDoc()
{
	if (m_outline)
	{
		fz_free_outline(m_context, m_outline);
		m_outline = nullptr;
	}
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
	m_context = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
	if (!m_context)
	{
		return E_OUTOFMEMORY;
	}
	else
	{
		return S_OK;
	}
}
//
//static MuPDFAlertIconType GetMuPDFAlertIconType(int iconType)
//{
//	switch (iconType)
//	{
//	case FZ_ALERT_ICON_ERROR:
//		return ALERTICONERROR;
//	case FZ_ALERT_ICON_WARNING:
//		return ALERTICONWARNING;
//	case FZ_ALERT_ICON_QUESTION:
//		return ALERTICONQUESTION;
//	case FZ_ALERT_ICON_STATUS:
//		return ALERTICONSTATUS;
//	default:
//		//!!!
//		return (MuPDFAlertIconType)iconType;
//	};
//}
//
//static MuPDFAlertButtonGroupType GetMuPDFAlertButtonGroupType(int buttonGroupType)
//{
//	switch (buttonGroupType)
//	{
//	case FZ_ALERT_BUTTON_GROUP_OK:
//		return ALERTBUTTONGROUPOK;
//	case FZ_ALERT_BUTTON_GROUP_OK_CANCEL:
//		return ALERTBUTTONGROUPOKCANCEL;
//	case FZ_ALERT_BUTTON_GROUP_YES_NO:
//		return ALERTBUTTONGROUPYESNO;
//	case FZ_ALERT_BUTTON_GROUP_YES_NO_CANCEL:
//		return ALERTBUTTONGROUPYESNOCANCEL;
//	default:
//		//!!!
//		return (MuPDFAlertButtonGroupType)buttonGroupType;
//	}
//}
//
//static int GetButtonPressed(MuPDFAlertButtonPressed buttonPressed)
//{
//	switch (buttonPressed)
//	{
//	case ALERTBUTTONNONE:
//		return FZ_ALERT_BUTTON_NONE;
//	case ALERTBUTTONOK:
//		return FZ_ALERT_BUTTON_OK;
//	case ALERTBUTTONCANCEL:
//		return FZ_ALERT_BUTTON_CANCEL;
//	case ALERTBUTTONNO:
//		return FZ_ALERT_BUTTON_NO;
//	case ALERTBUTTONYES:
//		return FZ_ALERT_BUTTON_YES;
//	default:
//		return (int)buttonPressed;
//	}
//}

static std::unique_ptr<char[]> CopyToUniqueStr(char *str)
{
	size_t len = strlen(str);
	std::unique_ptr<char[]> copyStr(new char[len + 1]);
	strcpy_s(copyStr.get(), len + 1, str);
	return copyStr;
}

//static std::shared_ptr<MuPDFAlertEvent> CreateMuPDFAlertEvent(const fz_alert_event* alert)
//{
//	std::shared_ptr<MuPDFAlertEvent> alertEvent(new MuPDFAlertEvent());
//	alertEvent->message = CopyToUniqueStr(alert->message);
//	alertEvent->iconType = GetMuPDFAlertIconType(alert->icon_type);
//	alertEvent->buttonGroupType = GetMuPDFAlertButtonGroupType(alert->button_group_type);
//	alertEvent->title = CopyToUniqueStr(alert->title);
//	alertEvent->checkBoxMessage = CopyToUniqueStr(alert->check_box_message);
//	alertEvent->initiallyChecked = alert->initially_checked == 0 ? false : true;
//	return alertEvent;
//}
//
//void MuPDFDoc::ShowAlert(fz_alert_event* alert)
//{
//	std::shared_ptr<MuPDFAlertEvent> alertEvent = CreateMuPDFAlertEvent(alert);
//	if (m_alertEventCallback)
//	{
//		m_alertEventCallback(alertEvent);
//		alert->finally_checked = alertEvent->finallyChecked;
//		alert->button_pressed = GetButtonPressed(alertEvent->buttonPressed);		
//	}
//}
//
//void MuPDFDoc::EventCallback(fz_doc_event* event, void* data)
//{
//	MuPDFDoc* doc = (MuPDFDoc*)data;
//	
//	switch (event->type)
//	{
//	case FZ_DOCUMENT_EVENT_ALERT:
//		doc->ShowAlert(fz_access_alert_event(event));
//		break;
//	}
//}
//
//void MuPDFDoc::AlertsInit()
//{
//	fz_interactive* idoc = fz_interact(m_document);
//	if (!idoc)
//		return;
//	fz_set_doc_event_callback(idoc, EventCallback, this);
//}

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
			m_outline = fz_load_outline(m_document);
			//AlertsInit();
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
	pageCache->pageList = nullptr;
	fz_free_display_list(m_context, pageCache->annotList);
	pageCache->annotList = nullptr;
	fz_free_page(m_document, pageCache->page);
	pageCache->page = nullptr;
	fz_free_page(m_document, pageCache->hqPage);
	pageCache->hqPage = nullptr;
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
		fz_matrix ctm = CalcConvertMatrix();
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

int MuPDFDoc::GetPageWidth()
{
	return m_pages[m_currentPage].width;
}

int MuPDFDoc::GetPageHeight()
{
	return m_pages[m_currentPage].height;
}

bool MuPDFDoc::AuthenticatePassword(char *password)
{
	return fz_authenticate_password(m_document, password) != 0;
}

//void MuPDFDoc::ClearHQPages()
//{
//	for (int i = 0; i < NUM_CACHE; i++) 
//	{
//		fz_free_page(m_document, m_pages[i].hqPage);
//		m_pages[i].hqPage = NULL;
//	}
//}

HRESULT MuPDFDoc::DrawPage(unsigned char *bitmap, int x, int y, int width, int height, bool invert)
{
	fz_device *dev = nullptr;
	fz_pixmap *pixmap = nullptr;
	fz_var(dev);
	fz_var(pixmap);
	PageCache *pageCache = &m_pages[m_currentPage];
	fz_try(m_context)
	{
		fz_interactive *idoc = fz_interact(m_document);
		// Call fz_update_page now to ensure future calls yield the
		// changes from the current state
		if (idoc)
			fz_update_page(idoc, pageCache->page);

		if (!pageCache->pageList)
		{
			/* Render to list */
			pageCache->pageList = fz_new_display_list(m_context);
			dev = fz_new_list_device(m_context, pageCache->pageList);
			fz_run_page_contents(m_document, pageCache->page, dev, fz_identity, nullptr);
		}
		if (!pageCache->annotList)
		{
			if (dev)
			{
				fz_free_device(dev);
				dev = nullptr;
			}
			pageCache->annotList = fz_new_display_list(m_context);
			dev = fz_new_list_device(m_context, pageCache->annotList);
			for (fz_annot *annot = fz_first_annot(m_document, pageCache->page); annot; annot = fz_next_annot(m_document, annot))
				fz_run_annot(m_document, pageCache->page, annot, dev, fz_identity, nullptr);
		}
		fz_bbox rect;
		rect.x0 = x;
		rect.y0 = y;
		rect.x1 = x + width;
		rect.y1 = y + height;
		pixmap = fz_new_pixmap_with_bbox_and_data(m_context, fz_device_rgb, rect, bitmap);
		if (!pageCache->pageList && !pageCache->annotList)
		{
			fz_clear_pixmap_with_value(m_context, pixmap, 0xd0);
			break;
		}
		fz_clear_pixmap_with_value(m_context, pixmap, 0xff);
		//
		fz_matrix ctm = CalcConvertMatrix();
		fz_bbox bbox = fz_round_rect(fz_transform_rect(ctm, pageCache->mediaBox));
		/* Now, adjust ctm so that it would give the correct page width
		 * heights. */
		float xscale = (float)width/(float)(bbox.x1-bbox.x0);
		float yscale = (float)height/(float)(bbox.y1-bbox.y0);
		ctm = fz_concat(ctm, fz_scale(xscale, yscale));
		bbox = fz_round_rect(fz_transform_rect(ctm, pageCache->mediaBox));
		if (dev)
		{
			fz_free_device(dev);
			dev = nullptr;
		}
		dev = fz_new_draw_device(m_context, pixmap);
		if (pageCache->pageList)
			fz_run_display_list(pageCache->pageList, dev, ctm, bbox, nullptr);
		if (pageCache->annotList)
			fz_run_display_list(pageCache->annotList, dev, ctm, bbox, nullptr);
		fz_free_device(dev);
		dev = nullptr;
		if (invert)
			fz_invert_pixmap(m_context, pixmap);
		fz_drop_pixmap(m_context, pixmap);
	}
	fz_catch(m_context)
	{
		if (dev)
		{
			fz_free_device(dev);
			dev = nullptr;
		}
		if (pixmap)
		{
			fz_drop_pixmap(m_context, pixmap);
		}
		return E_FAIL;
	}
	return S_OK;
}

HRESULT MuPDFDoc::UpdatePage(int pageNumber, unsigned char *bitmap, int x, int y, int width, int height, bool invert)
{
	int index = FindPageInCache(pageNumber);
	if (index < 0)
	{
		HRESULT result = GotoPage(pageNumber);
		if (FAILED(result))
		{
			return result;
		}
		return DrawPage(bitmap, x, y, width, height, invert);
	}
	fz_device *dev = nullptr;
	fz_pixmap *pixmap = nullptr;
	fz_var(dev);
	fz_var(pixmap);
	PageCache *pageCache = &m_pages[m_currentPage];
	fz_try(m_context)
	{
		fz_interactive *idoc = fz_interact(m_document);
		// Call fz_update_page now to ensure future calls yield the
		// changes from the current state
		if (idoc)
			fz_update_page(idoc, pageCache->page);

		if (!pageCache->pageList)
		{
			/* Render to list */
			pageCache->pageList = fz_new_display_list(m_context);
			dev = fz_new_list_device(m_context, pageCache->pageList);
			fz_run_page_contents(m_document, pageCache->page, dev, fz_identity, nullptr);
		}
		if (!pageCache->annotList)
		{
			if (dev)
			{
				fz_free_device(dev);
				dev = nullptr;
			}
			pageCache->annotList = fz_new_display_list(m_context);
			dev = fz_new_list_device(m_context, pageCache->annotList);
			for (fz_annot *annot = fz_first_annot(m_document, pageCache->page); annot; annot = fz_next_annot(m_document, annot))
				fz_run_annot(m_document, pageCache->page, annot, dev, fz_identity, nullptr);
		}
		fz_bbox rect;
		rect.x0 = x;
		rect.y0 = y;
		rect.x1 = x + width;
		rect.y1 = y + height;
		pixmap = fz_new_pixmap_with_bbox_and_data(m_context, fz_device_rgb, rect, bitmap);
		//
		fz_matrix ctm = CalcConvertMatrix();
		fz_bbox bbox = fz_round_rect(fz_transform_rect(ctm, pageCache->mediaBox));
		/* Now, adjust ctm so that it would give the correct page width
		 * heights. */
		float xscale = (float)width/(float)(bbox.x1-bbox.x0);
		float yscale = (float)height/(float)(bbox.y1-bbox.y0);
		ctm = fz_concat(ctm, fz_scale(xscale, yscale));
		bbox = fz_round_rect(fz_transform_rect(ctm, pageCache->mediaBox));
		if (dev)
		{
			fz_free_device(dev);
			dev = nullptr;
		}
		fz_annot *annot;
		while (idoc && (annot = fz_poll_changed_annot(idoc, pageCache->page)) != NULL)
		{
			fz_bbox abox = fz_round_rect(fz_transform_rect(ctm, fz_bound_annot(m_document, annot)));
			abox = fz_intersect_bbox(abox, rect);

			if (!fz_is_empty_bbox(abox))
			{
				fz_clear_pixmap_rect_with_value(m_context, pixmap, 0xff, abox);
				dev = fz_new_draw_device_with_bbox(m_context, pixmap, abox);
				if (pageCache->pageList)
					fz_run_display_list(pageCache->pageList, dev, ctm, abox, nullptr);
				if (pageCache->annotList)
					fz_run_display_list(pageCache->annotList, dev, ctm, abox, nullptr);
				fz_free_device(dev);
				dev = nullptr;
				if (invert)
					fz_invert_pixmap_rect(pixmap, abox);
			}
		}
		fz_drop_pixmap(m_context, pixmap);
	}
	fz_catch(m_context)
	{
		if (dev)
		{
			fz_free_device(dev);
			dev = nullptr;
		}
		if (pixmap)
		{
			fz_drop_pixmap(m_context, pixmap);
		}
		return E_FAIL;
	}
	return S_OK;
}

//int MuPDFDoc::CountOutlineitems(fz_outline *outline)
//{
//	int count = 0;
//
//	while (outline)
//	{
//		if (outline->dest.kind == FZ_LINK_GOTO
//				&& outline->dest.ld.gotor.page >= 0
//				&& outline->title)
//		{
//			count++;
//		}
//		count += CountOutlineItems(outline->down);
//		outline = outline->next;
//	}
//	return count;
//}
//

int MuPDFDoc::FillOutline(
	std::shared_ptr<std::vector<std::shared_ptr<Outlineitem>>> items, 
	int position, 
	fz_outline *outline, 
	int level)
{
	while (outline)
	{
		if (outline->dest.kind == FZ_LINK_GOTO)
		{
			int pageNumber = outline->dest.ld.gotor.page;
			if (pageNumber >= 0 && outline->title)
			{
				std::shared_ptr<Outlineitem> item(new Outlineitem());
				item->level = level;
				item->pageNumber = pageNumber;
				item->title = CopyToUniqueStr(outline->title);
				items->push_back(item); 
				position++;
			}
		}
		position = FillOutline(items, position, outline->down, level + 1);
		if (position < 0) 
			return -1;
		outline = outline->next;
	}
	return position;
}

std::shared_ptr<std::vector<std::shared_ptr<Outlineitem>>> MuPDFDoc::GetOutline()
{
	std::shared_ptr<std::vector<std::shared_ptr<Outlineitem>>> items(new std::vector<std::shared_ptr<Outlineitem>>());
	FillOutline(items, 0, m_outline, 0);
	return items;
}

static std::shared_ptr<MuPDFDocLink> CreateLink(const fz_rect &rect)
{
	std::shared_ptr<MuPDFDocLink> docLink(new MuPDFDocLink());
	docLink->left = rect.x0;
	docLink->top = rect.y0;
	docLink->right = rect.x1;
	docLink->bottom = rect.y1;
	return docLink;
}

static std::shared_ptr<MuPDFDocLink> CreateInternalLink(const fz_link *link, fz_rect &rect)
{
	std::shared_ptr<MuPDFDocLink> docLink(CreateLink(rect));
	docLink->type = INTERNAL;
	docLink->internalPageNumber = link->dest.ld.gotor.page;
	return docLink;
}

static std::shared_ptr<MuPDFDocLink> CreateRemoteLink(const fz_link *link, fz_rect &rect)
{
	std::shared_ptr<MuPDFDocLink> docLink(CreateLink(rect));
	docLink->type = REMOTE;
	docLink->remotePageNumber = link->dest.ld.gotor.page;
	docLink->newWindow = link->dest.ld.gotor.new_window != 0 ? true : false;
	docLink->fileSpec = CopyToUniqueStr(link->dest.ld.gotor.file_spec);
	return docLink;
}

std::shared_ptr<MuPDFDocLink> CreateURILink(const fz_link *link, fz_rect &rect)
{
	std::shared_ptr<MuPDFDocLink> docLink(CreateLink(rect));
	docLink->type = URI;
	docLink->uri = CopyToUniqueStr(link->dest.ld.uri.uri);	
	return docLink;
}

std::shared_ptr<std::vector<std::shared_ptr<MuPDFDocLink>>> MuPDFDoc::GetLinks()
{
	PageCache *pageCache = &m_pages[m_currentPage];
	std::shared_ptr<std::vector<std::shared_ptr<MuPDFDocLink>>> links(new std::vector<std::shared_ptr<MuPDFDocLink>>());
	fz_matrix ctm = CalcConvertMatrix();
	fz_link* list = fz_load_links(m_document, pageCache->page);
	for(fz_link* link = list; link; link = link->next)
	{
		fz_rect rect = fz_transform_rect(ctm, link->rect);
		std::shared_ptr<MuPDFDocLink> docLink;
		switch (link->dest.kind)
		{
		case FZ_LINK_GOTO:
		{
			docLink = CreateInternalLink(link, rect);
			break;
		}

		case FZ_LINK_GOTOR:
		{
			docLink = CreateRemoteLink(link, rect);
			break;
		}

		case FZ_LINK_URI:
		{
			docLink = CreateURILink(link, rect);
			break;
		}

		default:
			continue;
		}
		links->push_back(docLink);
	}
	fz_drop_link(m_context, list);
	return links;
}

fz_matrix MuPDFDoc::CalcConvertMatrix()
{
	// fz_bound_page determine the size of a page at 72 dpi.
	float zoom = m_resolution / 72.0;
	return fz_scale(zoom, zoom);
}

//void MuPDFDoc::DumpAnnotationDisplayLists()
//{
//	for (int i = 0; i < NUM_CACHE; i++)
//	{
//		if (m_pages[i].annotList)
//		{
//			fz_free_display_list(m_context, m_pages[i].annotList);
//			m_pages[i].annotList = nullptr;
//		}
//	}
//}
//
//HRESULT MuPDFDoc::PassClickEvent(float x, float y, bool& changed)
//{
//	changed = false;
//	fz_interactive *idoc = fz_interact(m_document);
//	if (!idoc)
//		return S_OK;
//	PageCache *pageCache = &m_pages[m_currentPage];	
//	fz_matrix ctm = CalcConvertMatrix();
//	ctm = fz_invert_matrix(ctm);
//	fz_point p;
//	p.x = x;
//	p.y = y;
//	p = fz_transform_point(ctm, p);
//	fz_try(m_context)
//	{
//		fz_ui_event event;
//		event.etype = FZ_EVENT_TYPE_POINTER;
//		event.event.pointer.pt = p;
//		event.event.pointer.ptype = FZ_POINTER_DOWN;
//		changed = fz_pass_event(idoc, pageCache->page, &event) == 0 ? false : true;
//		event.event.pointer.ptype = FZ_POINTER_UP;
//		changed |= fz_pass_event(idoc, pageCache->page, &event) == 0 ? false : true;
//		if (changed) 
//		{
//			DumpAnnotationDisplayLists();
//		}
//	}
//	fz_catch(m_context)
//	{
//		return E_FAIL;
//	}
//	return S_OK;
//}