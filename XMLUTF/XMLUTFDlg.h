//////////////////////////////////////////////////////////////////////
// XMLUTFDlg.h
//
// Refer to UTFXMLDlg.cpp for software documentation
//////////////////////////////////////////////////////////////////////

#ifndef _XML_UTF_DIALOG_H_INCLUDED_
#define _XML_UTF_DIALOG_H_INCLUDED_

#include "store.h"
#include <map>


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#define FREELANCER_KEYPATH "SOFTWARE\\Microsoft\\Microsoft Games\\Freelancer\\1.0"

#define LEAF   (1<<7)
#define BRANCH (1<<4)

#define XML_BUFFER_SIZE  65536
#define RELEASE_ATTR(attr) if (attr) { delete attr; attr = NULL; }

#define STREQ(s1, s2)  (strcmp(s1, s2) == 0)
#define STRIEQ(s1, s2) (stricmp(s1, s2) == 0)

typedef std::map<DWORD, int> StrMap;
typedef StrMap::const_iterator StrIter;


//////////////////////////////////////////////////////////////////////
// Data Structures
//////////////////////////////////////////////////////////////////////

typedef struct
{
	char UTF[4];			// "UTF "
	DWORD version;			// 0x04
	DWORD tree_offset;		// 0x08
	DWORD tree_size;		// 0x0c
	DWORD unk1; 			// 0x10
	DWORD node_size;		// 0x14
	DWORD string_offset;	// 0x18
	DWORD string_alloc; 	// 0x1c
	DWORD string_size;		// 0x20
	DWORD data_offset;		// 0x24
	DWORD unk2; 			// 0x28
	DWORD unk3; 			// 0x2c
	FILETIME timestamp; 	// 0x30
} UTFHeader;				// size=0x38

typedef struct
{
	DWORD next; 			// 0x00 : next node on same level (sibling)
	DWORD name; 			// 0x04 : string for this node
	DWORD flags;			// 0x08 : bit 4 set = branch, bit 7 set = leaf
	DWORD unk;	 			// 0x0c
	DWORD branch;			// 0x10 : offset to branch node (child), offset to data if leaf
	DWORD alloc_size;		// 0x14 : leaf node only, 0 for branch
	DWORD size1;			// 0x18 : leaf node only, 0 for branch
	DWORD size2;			// 0x1c : leaf node only, 0 for branch
	DWORD time1;			// 0x20
	DWORD time2;			// 0x24
	DWORD time3;			// 0x28
} UTFNode;					// size=0x2c

struct UTFNodeEntry;
struct UTFNodeEntry
{
    char *name;
    char *data_filename;
    char *data_path;
    BYTE *data;
	DWORD unk;
	DWORD time1;
	DWORD time2;
	DWORD time3;
    DWORD data_size;
    DWORD data_alloc;
    UTFNodeEntry *child;
    UTFNodeEntry *next;
};

struct VMeshData
{
	DWORD mesh_type;
	DWORD surf_type;
	 WORD mesh_count;
	 WORD index_count;
	 WORD FVF;
	 WORD vertex_count;
};

struct VWireData
{
	DWORD size;				// 16
	DWORD vmesh_hash;		// crc of vms name
	 WORD base;
	 WORD used;
	 WORD count;
	 WORD span;
};

typedef struct {
	DWORD header_size;		// 60
	DWORD vmesh_hash;		// crc of 3db name
    WORD  start_vert;
    WORD  end_vert;
    WORD  start_vert_ref;
    WORD  end_vert_ref;
    WORD  mesh_no;
    WORD  no_of_meshes;

    float bounding_box[6];
    float center[3];
    float radius;
} VMeshRef;

typedef struct 
{
	DWORD count;
	float interval;
	DWORD flags;
} Header;

struct Vector
{
	float x, y, z;
};

struct Matrix
{
	Vector i, j, k;
};

// \Cmpnd\Cons\{Fix,Trans,Loose}	// Trans isn't in any of the vanilla files
struct FIX
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Matrix orientation;
};

// \Cmpnd\Cons\{Pris,Rev}
struct REV
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  Vector axis;
  float  min, max;			// don't seem to be used
};

// \Cmpnd\Cons\Cyl
struct CYL				// not in any of the vanilla files
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  Vector axis;
  float  min1, max1;			// don't seem to be used
  float  min2, max2;
};

// \Cmpnd\Cons\Sphere
struct SPHERE
{
  char	 parent[64];
  char	 child[64];
  Vector position;
  Vector offset;
  Matrix orientation;
  float  min1, max1;			// don't seem to be used
  float  min2, max2;
  float  min3, max3;
};

// \Material Library\*\Frame rects
struct FrameRects
{
	DWORD frame;
	float rect[4];
};

// \ALEffectLib\ALEffectLib
struct ALEffectLib
{
	DWORD flag;
	DWORD CRC;
	DWORD parent;
	DWORD index;
};


DWORD fl_crc32(LPCSTR string);
DWORD fl_crc32_b(LPCSTR string);
DWORD CreateID(LPCSTR string);


//////////////////////////////////////////////////////////////////////
// XMLUTFDlg class definition
//////////////////////////////////////////////////////////////////////

class XMLUTFDlg : public CDialog
{
// Construction
public:
	XMLUTFDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(XMLUTFDlg)
	enum { IDD = IDD_XMLUTF_DIALOG };
	CComboBox	m_XmlFilenamesComboBox;
	CString	m_SourcePath;
	CString	m_DestinationPath;
	CString	m_XmlFilenames;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(XMLUTFDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(XMLUTFDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConvert();
	virtual void OnCancel();
	afx_msg void OnHelpButton();
	afx_msg void OnBrowseXmlFilename();
	afx_msg void OnSelchangeXmlFilenames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    UINT   RunBackgroundConversionThread();

private:
    void   ProcessFolder(LPCSTR folder);
    void   ProcessXmlFile(LPCSTR filename);
	void   OpenIncludeFile(LPCSTR filename);
	void   WriteUtfFile(LPCSTR filename, DWORD *unk, FILETIME *ft);
    void   ReadXML();
    void   SkipSpaces();
    void   SkipXmlTag();
    char * GetXmlName();
    char * GetXmlAttr(char *tag);
	bool   IsTag();
	bool   IsTag(LPCSTR tag, bool fail = false);
    void   CreateAndSetPath(const char * utf_path);
    void   Log(char *format, ...);
    void   ProcessXmlBranch(UTFNodeEntry * pParentNode);
    void   ProcessXmlLeaf(char *xml_name, char *name, char *type);
    void   DestroyTree(UTFNodeEntry * branch);
    void   ProcessText();
	void   ProcessByte();
	void   ProcessShort();
    void   ProcessInt(int size);
    void   ProcessInt();
    void   ProcessFloat(int size);
	void   ProcessFloat();
	void   ProcessRGB();
    void   ProcessHex(int size);
	void   ProcessVMeshData();
	void   ProcessVWireData();
	void   ProcessVMeshRef();
	void   ProcessHeader();
	void   ProcessFrames();
	void   ProcessFix();
	void   ProcessRev();
	void   ProcessCyl();
	void   ProcessSphere();
	void   ProcessFrame_rects();
	void   ProcessALEffectLib();
	void   ProcessAlchemyNodeLibrary();
	bool   do_Single(Store& data);
	DWORD  GetCRC(bool ignore = true);
    DWORD  GetDWORD();
    WORD   GetWORD();
	BYTE   GetBYTE();
    float  GetFloat();
    BYTE   GetHex();
	bool   GetByte(BYTE *byte);
	char * GetNumeric(bool is_float);
	char * GetString();
	void   GetVector(Vector& vec);
	void   GetMatrix(Matrix& mtx);
    int    ProcessUtfNode(UTFNodeEntry *node);
    int    AddUtfString(char *string);
    void   WriteUtfData(UTFNodeEntry *node);
	DWORD  GetHexAttr(char *attr);
	float  GetFloatAttr(char *attr);
	DWORD  GetTimeAttr(char *attr);
	bool   GetFileTimeAttr(char *attr, FILETIME &ft);
	void   EulerMatrix(const Vector &rot);

public:
    bool   m_ConversionAborted;
    bool   m_ConversionInProgress;
    bool   m_CountingXmlFiles;
    int    m_PathTrim;
    CRITICAL_SECTION m_Mutex;
    // The following public attributes are protected by the mutex
    int    m_XmlFileCount;
    int    m_NumXmlFilesOpened;
    int    m_NumUtfFilesCreated;
    char   m_CurrentSourcePath[MAX_PATH];
    // end of public attributes protected by the mutex

private:
    BOOL   m_Recursive;
    BOOL   m_CreateSubfolders;
    char   m_CurrentDestinationPath[MAX_PATH];
    int    m_Depth;
    char * m_XmlDataBuffer;
    char * m_XmlDataPtr;
    int    m_XmlLineNumber;
    UTFNodeEntry * m_UtfRoot;
    UTFNodeEntry * m_LeafNode;
    FILE * m_LogFile;
    FILE * m_XmlFile;
    FILE * m_UtfFile;
    bool   m_AbortXmlFileProcessing;
    DWORD  m_UtfNodeCount;
    int    m_UtfNodeIndex;
    int    m_UtfStringCount;
    int    m_UtfStringSize;
    int    m_UtfDataOffset;
    char **m_UtfStringArray;
    int  * m_UtfStringOffset;
	StrMap m_StrMap;
    UTFNode *m_UtfNode;
    DWORD  m_UtfFileSize;
	bool   m_StringFirst;
	bool   m_PrePadData;
	DWORD  m_time1, m_time2, m_time3;
    char * m_ExporterVersion;
	Header * m_AnimHeader;
	char * m_string;
	size_t m_strcap;
	bool   m_deg;
	float  m_matrix[9];
	int	   m_mtxidx;
	bool   m_Quiet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif _XML_UTF_DIALOG_H_INCLUDED_


