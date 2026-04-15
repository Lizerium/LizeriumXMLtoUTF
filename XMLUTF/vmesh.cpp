/******************************************************************************
 * vmesh.cpp
 *
 *
 *		Freelancer XML to UTF Conversion Utility.
 *
 *		Converts the various mesh nodes.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"
#include "store.h"
#include <set>


void XMLUTFDlg::ProcessVMeshData()
{
	Store data;
	VMeshData vmd;

	vmd.mesh_type = GetHexAttr("format");
	vmd.surf_type = GetHexAttr("surface");
	put(data, vmd);

	SkipXmlTag();

	if (!IsTag("mesh", true))
	{
		return;
	}
	SkipXmlTag();

	// Ignore the count, process as many as exist.
	vmd.mesh_count = 0;
	while (!IsTag())
	{
		 WORD first = GetWORD();
		 WORD last	= GetWORD();
		 WORD count = GetWORD();
		 WORD pad	= GetWORD();
		DWORD crc	= GetCRC();
		put(data, crc);
		put(data, first);
		put(data, last);
		put(data, count);
		put(data, pad);
		++vmd.mesh_count;
	}
	if (!IsTag("/mesh", true))
	{
		return;
	}
	SkipXmlTag();

	if (!IsTag("index", true))
	{
		return;
	}
	SkipXmlTag();

	int start = m_XmlLineNumber;
	vmd.index_count = 0;
	while (!IsTag())
	{
		put(data, GetWORD());
		++vmd.index_count;
	}
	int mod = vmd.index_count % 3;
	if (mod)
	{
		Log("   Warning: indices not a multiple of 3, lines %d-%d\n", start, m_XmlLineNumber);
		vmd.index_count -= mod;
		data.remove(mod * sizeof(WORD));
	}
	if (!IsTag("/index", true))
	{
		return;
	}
	SkipXmlTag();

	if (!IsTag("vertex", true))
	{
		return;
	}
	vmd.FVF = GetHexAttr("FVF");
	if (!(vmd.FVF & 2) || (vmd.FVF & ~0x352))
	{
		Log("   Error: missing or invalid FVF attribute, line %d\n", m_XmlLineNumber);
		m_AbortXmlFileProcessing = true;
		return;
	}
	SkipXmlTag();

	vmd.vertex_count = 0;
	while (!IsTag())
	{
		put(data, GetFloat());			// X
		put(data, GetFloat());			// Y
		put(data, GetFloat());			// Z
		if (vmd.FVF & 0x10)
		{
			put(data, GetFloat());		// X Normal
			put(data, GetFloat());		// Y Normal
			put(data, GetFloat());		// Z Normal
		}
		if (vmd.FVF & 0x40)
		{
			put(data, GetDWORD());		// Diffuse
		}
		if (vmd.FVF & 0x300)
		{
			put(data, GetFloat());		// U1
			put(data, GetFloat());		// V1
			if (vmd.FVF & 0x200)
			{
				put(data, GetFloat());	// U2
				put(data, GetFloat());	// V2
			}
		}
		++vmd.vertex_count;
	}
	if (!IsTag("/vertex", true))
	{
		return;
	}
	SkipXmlTag();

	put(data, vmd, 0);
	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessVWireData()
{
	Store data;
	VWireData vwd;
	std::set<int> used;
	WORD min = 65535, max = 0;

	vwd.size = sizeof(vwd);
	GetDWORD();
	vwd.vmesh_hash = GetCRC();
	vwd.base = GetWORD();
	/* vwd.used = */ GetWORD(); // determine automatically
	vwd.count = 0;	 GetWORD(); // determine automatically
	/* vwd.span = */ GetWORD(); // determine automatically

	put(data, vwd);
	int start = m_XmlLineNumber;
	while (!IsTag())
	{
		WORD vtx = GetWORD();
		put(data, vtx);
		if (vtx < min) min = vtx;
		if (vtx > max) max = vtx;
		used.insert(vtx);
		++vwd.count;
	}
	if (vwd.count & 1)
	{
		Log("   Warning: odd number of vertices, lines %d-%d\n", start, m_XmlLineNumber);
		--vwd.count;
		data.remove(sizeof(WORD));
	}
	vwd.used = used.size();
	vwd.span = max - min + 1;
	put(data, vwd, 0);
	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessVMeshRef()
{
	VMeshRef * data = new VMeshRef;
	m_LeafNode->data_size  =
    m_LeafNode->data_alloc = sizeof(VMeshRef);
    m_LeafNode->data = (BYTE *) data;

	data->header_size	  = GetDWORD();
	data->vmesh_hash	  = GetCRC();
	data->start_vert	  = GetWORD();
	data->end_vert		  = GetWORD();
	data->start_vert_ref  = GetWORD();
	data->end_vert_ref	  = GetWORD();
	data->mesh_no		  = GetWORD();
	data->no_of_meshes	  = GetWORD();

	data->bounding_box[0] = GetFloat();
	data->bounding_box[2] = GetFloat();
	data->bounding_box[4] = GetFloat();
	data->bounding_box[1] = GetFloat();
	data->bounding_box[3] = GetFloat();
	data->bounding_box[5] = GetFloat();

	data->center[0] 	  = GetFloat();
	data->center[1] 	  = GetFloat();
	data->center[2] 	  = GetFloat();

	data->radius		  = GetFloat();
}
