/******************************************************************************
 * cons.cpp
 *
 *
 *		Freelancer XML to UTF Conversion Utility.
 *
 *		Converts the various Cons nodes.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"


void XMLUTFDlg::ProcessFix()
{
	FIX   fix;
	Store data;

	fix.parent[63] = fix.child[63] = 0;
	while (IsTag("part"))
	{
		SkipXmlTag();

		strncpy(fix.parent, GetString(), 63);
		strncpy(fix.child,	GetString(), 63);
		GetVector(fix.position);
		GetMatrix(fix.orientation);

		if (!IsTag("/part", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, fix);
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessRev()
{
	REV   rev;
	Store data;

	rev.parent[63] = rev.child[63] = 0;
	while (IsTag("part"))
	{
		SkipXmlTag();

		strncpy(rev.parent, GetString(), 63);
		strncpy(rev.child,	GetString(), 63);
		GetVector(rev.position);
		GetVector(rev.offset);
		GetMatrix(rev.orientation);
		GetVector(rev.axis);
		rev.min = GetFloat();
		rev.max = GetFloat();

		if (!IsTag("/part", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, rev);
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessCyl()
{
	CYL   cyl;
	Store data;

	cyl.parent[63] = cyl.child[63] = 0;
	while (IsTag("part"))
	{
		SkipXmlTag();

		strncpy(cyl.parent, GetString(), 63);
		strncpy(cyl.child,	GetString(), 63);
		GetVector(cyl.position);
		GetVector(cyl.offset);
		GetMatrix(cyl.orientation);
		GetVector(cyl.axis);
		cyl.min1 = GetFloat();
		cyl.max1 = GetFloat();
		cyl.min2 = GetFloat();
		cyl.max2 = GetFloat();

		if (!IsTag("/part", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, cyl);
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessSphere()
{
	SPHERE sphere;
	Store  data;

	sphere.parent[63] = sphere.child[63] = 0;
	while (IsTag("part"))
	{
		SkipXmlTag();

		strncpy(sphere.parent, GetString(), 63);
		strncpy(sphere.child,  GetString(), 63);
		GetVector(sphere.position);
		GetVector(sphere.offset);
		GetMatrix(sphere.orientation);
		sphere.min1 = GetFloat();
		sphere.max1 = GetFloat();
		sphere.min2 = GetFloat();
		sphere.max2 = GetFloat();
		sphere.min3 = GetFloat();
		sphere.max3 = GetFloat();

		if (!IsTag("/part", true))
		{
			return;
		}
		SkipXmlTag();

		put(data, sphere);
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}
