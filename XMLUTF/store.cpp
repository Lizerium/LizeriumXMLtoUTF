/*
  store.cpp - Dump arbitrary data into a memory buffer.

  Jason Hood, 17 February, 2010.
*/

#include "stdafx.h"
#include "store.h"


// Write the length (including NUL) of the string as a word, followed by the
// string, followed by an additional NUL if necessary for WORD alignment.
void Store::put( LPCSTR str )
{
  int len = strlen( str ) + 1;
  int pad = len & 1;
  int size = 2 + len + pad;

  if (m_size + size > m_capacity)
  {
	size_t new_size = ((m_size + size) | 127) + 1;
	char* temp = new char[new_size];
	memcpy( temp, m_data, m_size );
	delete m_data;
	m_data = temp;
	m_capacity = new_size;
  }

  memcpy( m_data + m_size, &len, 2 );
  memcpy( m_data + m_size + 2, str, len );
  if (pad)
	m_data[m_size+2+len] = 0;

  m_size += size;
}


void Store::put( const void* data, size_t size )
{
  if (m_size + size > m_capacity)
  {
	size_t new_size = ((m_size + size) | 127) + 1;
	char* temp = new char[new_size];
	memcpy( temp, m_data, m_size );
	delete m_data;
	m_data = temp;
	m_capacity = new_size;
  }

  memcpy( m_data + m_size, data, size );
  m_size += size;
}


// Align the size to a multiple of four, padding with zeros.
size_t Store::align()
{
  int pad = size() & 3;
  if (pad)
  {
    DWORD zero = 0;
	put( &zero, 4 - pad );
  }

  return m_size;
}
