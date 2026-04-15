/*
  store.h - Dump arbitrary data into a memory buffer.

  Jason Hood, 17 February, 2010.
*/

#ifndef _STORE_H
#define _STORE_H

class Store
{
  char*  m_data;
  size_t m_size;
  size_t m_capacity;
  bool	 m_keep;

public:
  Store() : m_data( 0 ), m_size( 0 ), m_capacity( 0 ), m_keep( false ) { }

  // Tidy up if processing is aborted.
  ~Store()
  {
	if (!m_keep)
	  delete m_data;
  }

  void put( LPCSTR );
  void put( const void*, size_t );

  void put( const void* data, size_t size, size_t pos )
  {
	// Assumes overwriting previous data, not jumping ahead!
	memcpy( m_data + pos, data, size );
  }

  void remove( size_t len )
  {
	if (len >= m_size)
	  m_size -= len;
  }

  size_t align();

  size_t size() const { return m_size; }

  char*  data() 
  { 
	m_keep = true; 
    return m_data; 
  }
};


template <class T>
inline void put( Store& store, const T& data )
{
  store.put( &data, sizeof(T) );
}


template <class T>
inline void put( Store& store, const T& data, size_t pos )
{
  store.put( &data, sizeof(T), pos );
}

#endif
