#if !defined( __I3_HASH_H)
#define __I3_HASH_H

#include "i3CommonType.h"

namespace i3Hash
{
	// http://www.partow.net/programming/hashfunctions/

	//////////////////////////////////////////////////////////////////////
	// RS Hash Function
	// A simple hash function from Robert Sedgwicks Algorithms in C book.
	// I've added some simple optimizations to the algorithm in order to speed up its hashing process. 
	I3_EXPORT_BASE UINT32	RS( char * pBuf, INT32 len);


	//////////////////////////////////////////////////////////////////////
	// JS Hash Function
	// A bitwise hash function written by Justin Sobel 
	I3_EXPORT_BASE UINT32	JS( char * pBuf, INT32 len);
	

	//////////////////////////////////////////////////////////////////////
	// PJW Hash Function
	// This hash algorithm is based on work by Peter J. Weinberger of AT&T Bell Labs.
	// The book Compilers (Principles, Techniques and Tools) by Aho, Sethi and Ulman, 
	// recommends the use of hash functions that employ the hashing methodology found in this particular algorithm. 
	I3_EXPORT_BASE UINT32	PJW( char * pBuf, INT32 len);


	//////////////////////////////////////////////////////////////////////
	// ELF Hash Function
	// Similar to the PJW Hash function, but tweaked for 32-bit processors.
	// Its the hash function widely used on most UNIX systems. 
	I3_EXPORT_BASE UINT32	ELF( char * pBuf, INT32 len);


	//////////////////////////////////////////////////////////////////////
	// BKDR Hash Function
	// This hash function comes from Brian Kernighan and Dennis Ritchie's book "The C Programming Language".
	// It is a simple hash function using a strange set of possible seeds which all constitute a pattern of 31....31...31 etc, 
	// it seems to be very similar to the DJB hash function. 
	I3_EXPORT_BASE UINT32	BKDR( char * pBuf, INT32 len);


	//////////////////////////////////////////////////////////////////////
	// SDBM Hash Function
	// This is the algorithm of choice which is used in the open source SDBM project.
	// The hash function seems to have a good over-all distribution for many different data sets.
	// It seems to work well in situations where there is a high variance in the MSBs of the elements in a data set. 
	I3_EXPORT_BASE UINT32	SDBM( char * pBuf, INT32 len);


	//////////////////////////////////////////////////////////////////////
	// DJB Hash Function
	// An algorithm produced by Professor Daniel J. Bernstein and shown first to the world
	// on the usenet newsgroup comp.lang.c. 
	// It is one of the most efficient hash functions ever published. 
	I3_EXPORT_BASE UINT32	DJB( char * pBuf, INT32 len);


	///////////////////////////////////////////////////////////////////////
	// DEK Hash Function
	// An algorithm proposed by Donald E. Knuth in The Art Of Computer Programming Volume 3,
	// under the topic of sorting and search chapter 6.4. 
	I3_EXPORT_BASE UINT32	DEK( char * pBuf, INT32 len);


	I3_EXPORT_BASE UINT32	BP( char * pBuf, INT32 len);


	I3_EXPORT_BASE UINT32	FNV( char * pBuf, INT32 len);


	////////////////////////////////////////////////////////////////////////////////
	// AP Hash Function
	// An algorithm produced by me Arash Partow.
	// I took ideas from all of the above hash functions making a hybrid rotative and additive hash function algorithm.
	// There isn't any real mathematical analysis explaining why one should use this hash function
	// instead of the others described above other than the fact that I tired to resemble the design
	// as close as possible to a simple LFSR.
	// An empirical result which demonstrated the distributive abilities of the hash algorithm was
	// obtained using a hash-table with 100003 buckets, hashing The Project Gutenberg Etext of Webster's Unabridged Dictionary,
	// the longest encountered chain length was 7, the average chain length was 2, the number of empty buckets was 4579.
	I3_EXPORT_BASE UINT32	AP( char * pBuf, INT32 len);
};

#endif
