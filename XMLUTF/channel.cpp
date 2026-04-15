/******************************************************************************
 * channel.cpp
 *
 *
 *		Freelancer XML to UTF Conversion Utility.
 *
 *		Converts the Channel Header & Frames.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"


void XMLUTFDlg::ProcessHeader()
{
	m_AnimHeader = new Header;
	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = sizeof(Header);
	m_LeafNode->data = (BYTE *) m_AnimHeader;

	m_AnimHeader->count = 0;	// determine automatically
	GetDWORD();
	if (*m_XmlDataPtr == 'E')
	{
		GetString();
		m_AnimHeader->interval = -1;
	}
	else
	{
		m_AnimHeader->interval = GetFloat();
	}
	m_AnimHeader->flags = GetDWORD();
}


void XMLUTFDlg::ProcessFrames()
{
	if (!m_AnimHeader)
	{
		Log("   Error: Header must occur before Frames, line %d\n", m_XmlLineNumber);
		m_AbortXmlFileProcessing = true;
		return;
	}

	Store data;

	bool  t = (m_AnimHeader->interval < 0);
	DWORD f = m_AnimHeader->flags;
	while (!IsTag())
	{
		if (t)
		{
			put(data, GetFloat());
		}
		if (f & 1)
		{
			put(data, GetFloat());
		}
		if (f & 2)
		{
			put(data, GetFloat());
			put(data, GetFloat());
			put(data, GetFloat());
		}
		if (f & 4)
		{
			put(data, GetFloat());
			put(data, GetFloat());
			put(data, GetFloat());
			put(data, GetFloat());
		}
		if (f & 0xc0)
		{
			// I made UTFXML write the already-scaled version, leaving the
			// raw short version commented; let's allow either, by testing
			// for a point.
			if (strchr(m_XmlDataPtr, '.'))
			{
				float f = GetFloat() * 32767.0f;
				f += (f < 0) ? -0.5f : 0.5f;
				put(data, (short) f);
				f = GetFloat() * 32767.0f;
				f += (f < 0) ? -0.5f : 0.5f;
				put(data, (short) f);
				f = GetFloat() * 32767.0f;
				f += (f < 0) ? -0.5f : 0.5f;
				put(data, (short) f);
			}
			else
			{
				put(data, GetWORD());
				put(data, GetWORD());
				put(data, GetWORD());
			}
		}
		++m_AnimHeader->count;
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();

	m_AnimHeader = NULL;
}
