/******************************************************************************
 * ale.cpp
 *
 *
 *		Freelancer XML to UTF Conversion Utility.
 *
 *		Converts ALEffectLib & AlchemyNodeLibrary.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"
#include <float.h>


void XMLUTFDlg::ProcessALEffectLib()
{
	Store data;

	float ver = GetFloatAttr("version");
	if (ver != 1 && ver != 1.1f)
	{
		Log("   Error: invalid/missing effect version number.  Line %d\n", m_XmlLineNumber);
		m_AbortXmlFileProcessing = true;
		return;
	}
	put(data, ver);

	SkipXmlTag();

	DWORD effect_count = 0;
	put(data, effect_count);
	while (IsTag("effect"))
	{
		char *name = GetXmlAttr("name");
		if (!name)
		{
			Log("   Error: effect missing name.  Line %d\n", m_XmlLineNumber);
			m_AbortXmlFileProcessing = true;
			return;
		}
		data.put(name);
		delete name;
		SkipXmlTag();

		if (ver == 1.1f)
		{
			if (!IsTag("unused", true))
			{
				return;
			}
			SkipXmlTag();
			put(data, GetFloat());
			put(data, GetFloat());
			put(data, GetFloat());
			put(data, GetFloat());
			if (!IsTag("/unused", true))
			{
				return;
			}
			SkipXmlTag();
		}

		if (!IsTag("fx", true))
		{
			return;
		}
		SkipXmlTag();

		size_t pos = data.size();
		DWORD count = 0;
		put(data, count);
		ALEffectLib ael;
		while (!IsTag())
		{
			ael.index  = GetDWORD();
			ael.parent = GetDWORD();
			ael.flag   = GetDWORD();
			ael.CRC    = GetCRC(false);
			put(data, ael);
			++count;
		}
		if (!IsTag("/fx", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, count, pos);

		if (!IsTag("pairs", true))
		{
			return;
		}
		SkipXmlTag();

		pos = data.size();
		count = 0;
		put(data, count);
		while (!IsTag())
		{
			put(data, GetDWORD());
			put(data, GetDWORD());
			++count;
		}
		if (!IsTag("/pairs", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, count, pos);

		if (!IsTag("/effect", true))
		{
			return;
		}
		SkipXmlTag();

		++effect_count;
	}
	put(data, effect_count, 4);

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessAlchemyNodeLibrary()
{
	Store data;

	float ver = GetFloatAttr("version");
	if (ver != 1.1f)
	{
		Log("   Error: invalid/missing effect version number.  Line %d\n", m_XmlLineNumber);
		m_AbortXmlFileProcessing = true;
		return;
	}
	put(data, ver);

	SkipXmlTag();

	DWORD node_count = 0;
	put(data, node_count);
	while (IsTag("node"))
	{
		char *name = GetXmlAttr("name");
		if (!name)
		{
			Log("   Error: node missing name, line %d\n", m_XmlLineNumber);
			m_AbortXmlFileProcessing = true;
			return;
		}
		data.put(name);
		delete name;
		SkipXmlTag();

		WORD id;
		while (IsTag("effect"))
		{
			name = GetXmlAttr("type");
			if (name[0] == '0')
			{
				id = (WORD) strtoul(name, NULL, 0);
			}
			else
			{
				switch (*name)
				{
				case 'b': id = 0x001; break;
				case 'c': id = 0x201; break;
				case 'f': id = (name[1] == 'l') ? 0x003 : 0x200; break;
				case 'i': id = (name[1] == 'n') ? 0x002 : 0x104; break;
				case 'n': id = 0x103; break;
				case 'v': id = 0x202; break;
				case 'x': id = 0x105; break;
				default:
					Log("    Error: unknown node type, line %d\n", m_XmlLineNumber);
					m_AbortXmlFileProcessing = true;
					return;
				}
			}
			delete name;
			name = GetXmlAttr("name");
			DWORD crc;
			if (name)
			{
				if (STREQ(name, "BeamApp_LineAppearance"))
				{
					crc = 0x1C65B7B9;
				}
				else
				{
					crc = fl_crc32_b(name);
				}
				delete name;
			}
			else
			{
				crc = GetHexAttr("crc");
				if (!crc)
				{
					Log("   Error: missing effect name/crc, line %d\n", m_XmlLineNumber);
					m_AbortXmlFileProcessing = true;
					return;
				}
			}
			put(data, id);
			put(data, crc);

			SkipXmlTag();

			switch (id)
			{
			case 0x001 :
				if (STREQ(GetString(), "true"))
				{
					id |= 0x8000;
					put(data, id, data.size()-6);
				}
				break;

			case 0x002 :
				put(data, GetDWORD());
				break;

			case 0x003 :
				if (*m_XmlDataPtr == 'I')   // Infinite
				{
					GetString();
					put(data, FLT_MAX);
				}
				else
				{
					put(data, GetFloat());
				}
				break;

			case 0x103 :
				name = GetString();
				if (*name)
				{
					data.put(name);
				}
				else
				{
					id = 0;
					put(data, id);
				}
				break;

			case 0x104 :
				put(data, GetDWORD());
				put(data, GetDWORD());
				break;

			case 0x105 :
			{
				DWORD xfm = GetDWORD();
				BYTE b = (BYTE) (xfm >> 8);
				put(data, b);
				b = ((BYTE) xfm >> 4) & 15;
				put(data, b);
				b = (BYTE) xfm & 15;
				put(data, b);
				if (IsTag("single"))
				{
					b = 0x80;
					put(data, b);
					for (int j = 0; j < 9; ++j)
					{
						if (!do_Single(data))
						{
							return;
						}
					}
				}
				else
				{
					b = 0;
					put(data, b);
				}
				break;
			}

			case 0x200 :
			{
				if (!IsTag("float_header", true))
				{
					return;
				}
				BYTE type, count, fcount;
				size_t pos, fpos;
				type = (BYTE) GetHexAttr("type");
				put(data, type);
				count = 0;
				pos = data.size();
				put(data, count);
				SkipXmlTag();
				while (!IsTag())
				{
					put(data, GetFloat());
					if (!IsTag("float", true))
					{
						return;
					}
					type = (BYTE) GetHexAttr("type");
					put(data, type);
					fcount = 0;
					fpos = data.size();
					put(data, fcount);
					SkipXmlTag();
					while (!IsTag())
					{
						put(data, GetFloat());
						put(data, GetFloat());
						++fcount;
					}
					if (!IsTag("/float", true))
					{
						return;
					}
					SkipXmlTag();

					put(data, fcount, fpos);

					++count;
				}
				if (!IsTag("/float_header", true))
				{
					return;
				}
				SkipXmlTag();

				put(data, count, pos);
				break;
			}

			case 0x201 :
			{
				if (!IsTag("rgb_header", true))
				{
					return;
				}
				BYTE type, count, fcount;
				size_t pos, fpos;
				type = (BYTE) GetHexAttr("type");
				put(data, type);
				count = 0;
				pos = data.size();
				put(data, count);
				SkipXmlTag();
				while (!IsTag())
				{
					put(data, GetFloat());
					if (!IsTag("rgb", true))
					{
						return;
					}
					type = (BYTE) GetHexAttr("type");
					put(data, type);
					fcount = 0;
					fpos = data.size();
					put(data, fcount);
					SkipXmlTag();
					while (!IsTag())
					{
						put(data, GetFloat());
						Vector rgb;
						GetVector(rgb);
						if (rgb.x > 1 || rgb.y > 1 || rgb.z > 1)
						{
							rgb.x /= 255;
							rgb.y /= 255;
							rgb.z /= 255;
						}
						put(data, rgb);
						++fcount;
					}
					if (!IsTag("/rgb", true))
					{
						return;
					}
					SkipXmlTag();

					put(data, fcount, fpos);

					++count;
				}
				if (!IsTag("/rgb_header", true))
				{
					return;
				}
				SkipXmlTag();

				put(data, count, pos);
				break;
			}

			case 0x202 :
				if (!do_Single(data))
				{
					return;
				}
				break;

			default:
				Log("   Error: unknown effect type, line %d\n", m_XmlLineNumber);
				m_AbortXmlFileProcessing = true;
				return;
			}

			if (!IsTag("/effect", true))
			{
				return;
			}
			SkipXmlTag();
		}
		id = 0;
		put(data, id);

		if (!IsTag("/node", true))
		{
			return;
		}
		SkipXmlTag();

		++node_count;
	}
	put(data, node_count, 4);

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


bool XMLUTFDlg::do_Single(Store& data)
{
	if (!IsTag("single", true))
	{
		return false;
	}

	BYTE type, count;
	WORD loop, lcount;
	size_t pos, lpos;
	type = (BYTE) GetHexAttr("type");
	put(data, type);
	count = 0;
	pos = data.size();
	put(data, count);
	SkipXmlTag();
	while (!IsTag())
	{
		put(data, GetFloat());
		put(data, GetFloat());
		if (IsTag("loop"))
		{
			loop = (BYTE) GetHexAttr("type");
			put(data, loop);
			lcount = 0;
			lpos = data.size();
			put(data, lcount);
			SkipXmlTag();
			while (!IsTag())
			{
				put(data, GetFloat());
				put(data, GetFloat());
				put(data, GetFloat());
				put(data, GetFloat());
				++lcount;
			}
			if (!IsTag("/loop", true))
			{
				return false;
			}
			SkipXmlTag();

			put(data, lcount, lpos);
		}
		else
		{
			loop = 0;
			put(data, loop);
			put(data, loop);
		}

		++count;
	}
	if (!IsTag("/single", true))
	{
		return false;
	}
	SkipXmlTag();

	put(data, count, pos);

	return true;
}
