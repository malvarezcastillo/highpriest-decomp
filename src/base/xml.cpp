#include "common.h"
#include "xml.h"

#include <string.h>


using std::string;

XMLElement :: XMLElement(LPCSTR name) :
	m_name(name),m_next(NULL),m_child(NULL),m_start(NULL),m_end(NULL)
{
	sAssert( name && *name );
}

XMLElement :: ~XMLElement()
{
	Clear();
}

void XMLElement :: Clear()
{
	if(m_next)
	{
		delete m_next;
		m_next = NULL;
	}
	if(m_child)
	{
		delete m_child;
		m_child = NULL;
	}
	m_start = NULL;
	m_end = NULL;
}

string&	XMLElement :: GetContents()
{
	if ( m_contents.empty() )
	{
		sAssert( IsTag(m_start) );
		LPCSTR document = SkipTag(m_start,m_end);
		LPCSTR end = m_end;
		while(document != end)
		{
			XMLElement* child = m_child;
			while( child )
			{
				if ( document == child->m_start )
					document = child->m_end;
				sAssert(document >= m_start && document <= end);
				XMLElement* next = child->m_next;
				while ( next )
				{
					if ( document == next->m_start )
					{
						document = next->m_end;
					}
					sAssert(document >= m_start && document <= end);
					next = next->m_next;
				}
				child = child->m_child;
			}
			if(IsTag(document))
			{

				break;
			}
			if(*document == 0x0d || *document == 0x0a) {

				++document;
			} else if(IsSpecialCharater(document)) {
				m_contents.append(ParseSpecialCharater(document+1, end));
				document = SkipSpecialCharacter(document, end);
			} else {

				m_contents += *document;
				++document;
			}
			sAssert(document >= m_start && document <= end);
		}
	}

	return m_contents;
}

void XMLElement :: AddChild(XMLElement* element)
{
	if(!m_child) m_child = element;
	else m_child->AddNext(element);
}

void XMLElement :: AddNext(XMLElement* element)
{
	if(!m_next) m_next = element;
	else m_next->AddNext(element);
}

XMLElement*	XMLElement :: FindChild(LPCSTR name)
{
	if(!m_child) return NULL;

	if( strcmp(m_child->GetName(),name) == 0 )
		return m_child;
	else
		return m_child->FindNext(name);
}

XMLElement*	XMLElement :: FindNext(LPCSTR name)
{
	if(!m_next) return NULL;

	if( strcmp(m_next->GetName(),name) == 0 )
		return m_next;
	else
		return m_next->FindNext(name);
}

string	XMLElement :: ParseSpecialCharater(const char* document, const char* end)
{
	if(memcmp(document, "lt", 2) == 0) return string("<");
	else if(memcmp(document, "gt", 2) == 0) return string(">");
	else if(memcmp(document, "amp", 3) == 0) return string("&");
	else return string();
}

void	XMLDocument :: Clear()
{
	m_root.Clear();
	if ( m_buf )
	{
		delete [] m_buf;
		m_buf = NULL;
	}
}

bool	XMLDocument :: ReadDocument(const char* document, const char* end)
{
	if ( m_root.GetChild() )
	{
		Trace("Deleting the previous XMLDocument.!!");
		Clear();
	}
	int size = end - document;
	m_buf = new char[size+1];
#ifdef _DEBUG
	static int iMem = 0;
	iMem += size+1;
	Trace("ReadDocument Memory Usage : %d",iMem);
#endif
	memcpy(m_buf, document, size);
	m_buf[size] = NULL;

	m_root.SetStart(m_buf);
	m_root.SetEnd(m_buf+size);

	ReadContents(&m_root, m_buf, m_buf+size);

	return m_root.GetChild() ? true : false;
}



const char*	XMLDocument :: ReadContents(XMLElement* element, const char* document, const char* end)
{
	while(document != end)
	{
		if(IsTag(document))
		{
			if(IsEndTag(document))
			{

				const char * buf = SkipTag(document, end);
				if( strcmp( ParseTag(document+2, end),element->GetName() ) == 0 )
				{
					element->SetEnd(buf);
					return buf;
				} else
					document = buf;
			} else {

				document = ReadElement(element, document, end);
			}
		}
		else if(IsSpecialCharater(document))
			document = SkipSpecialCharacter(document, end);
		else
			++document;
		sAssert(document <= end);
	}
	return document;
}

const char*	XMLDocument :: ReadElement(XMLElement* parent, const char* document, const char* end)
{
	if(IsPITag(document))
		return SkipTag(document, end);
	else if(IsCommentTag(document))
		return SkipTag(document, end);
	else
	{

		const char * buf = SkipTag(document, end);
		XMLElement* element = new XMLElement(ParseTag(document+1, end));
		parent->AddChild(element);
		if( IsClosedTag(buf, end) )
			return buf;
		else
		{
			element->SetStart(document);
			return ReadContents(element, buf, end);
		}
	}
}

LPCSTR XMLDocument :: ParseTag(const char* document, const char* end)
{
	char* iter = (char *)document;
	while(iter != end && *iter != ' ' && *iter != '\t' && *iter != '>' && *iter != '/') ++iter;
	sAssertM(*iter == '>',"(xml-parsing problem)" );
	iter[0] = NULL;
	return document;
}
