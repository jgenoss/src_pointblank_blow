#include "i3CommonType.h"
#include "i3Hash.h"

// http://www.partow.net/programming/hashfunctions/

//////////////////////////////////////////////////////////////////////
// RS Hash Function
// A simple hash function from Robert Sedgwicks Algorithms in C book.
// I've added some simple optimizations to the algorithm in order to speed up its hashing process. 
UINT32 i3Hash::RS( char * pBuf, INT32 len)
{
	UINT32 b    = 378551;
	UINT32 a    = 63689;
	UINT32 hash = 0;

	for( INT32 i = 0; i < len; i++)
	{
		hash = hash * a + pBuf[i];
		a    = a * b;
	}

	return hash;
}
/* End Of RS Hash Function */

//////////////////////////////////////////////////////////////////////
// JS Hash Function
// A bitwise hash function written by Justin Sobel 
UINT32 i3Hash::JS( char * pBuf, INT32 len)
{
   UINT32 hash = 1315423911;

   for( INT32 i = 0; i < len; i++)
   {
      hash ^= ((hash << 5) + pBuf[i] + (hash >> 2));
   }

   return hash;
}
/* End Of JS Hash Function */


//////////////////////////////////////////////////////////////////////
// PJW Hash Function
// This hash algorithm is based on work by Peter J. Weinberger of AT&T Bell Labs.
// The book Compilers (Principles, Techniques and Tools) by Aho, Sethi and Ulman, 
// recommends the use of hash functions that employ the hashing methodology found in this particular algorithm. 
UINT32 i3Hash::PJW( char * pBuf, INT32 len)
{
   UINT32 BitsInUnsignedInt = (UINT32) (sizeof(UINT32) * 8);
   UINT32 ThreeQuarters     = (UINT32) ((BitsInUnsignedInt  * 3) / 4);
   UINT32 OneEighth         = (UINT32) (BitsInUnsignedInt / 8);
   UINT32 HighBits          = (UINT32) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   UINT32 hash              = 0;
   UINT32 test              = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash = (hash << OneEighth) + pBuf[i];

      if((test = hash & HighBits)  != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}
/* End Of  P. J. Weinberger Hash Function */

//////////////////////////////////////////////////////////////////////
// ELF Hash Function
// Similar to the PJW Hash function, but tweaked for 32-bit processors.
// Its the hash function widely used on most UNIX systems. 
UINT32 i3Hash::ELF( char * pBuf, INT32 len)
{
   UINT32 hash = 0;
   UINT32 x    = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash = (hash << 4) + pBuf[i];

      if((x = hash & 0xF0000000L) != 0)
      {
         hash ^= (x >> 24);
      }
      hash &= ~x;
   }

   return hash;
}
/* End Of ELF Hash Function */

//////////////////////////////////////////////////////////////////////
// BKDR Hash Function
// This hash function comes from Brian Kernighan and Dennis Ritchie's book "The C Programming Language".
// It is a simple hash function using a strange set of possible seeds which all constitute a pattern of 31....31...31 etc, 
// it seems to be very similar to the DJB hash function. 

UINT32 i3Hash::BKDR( char * pBuf, INT32 len)
{
   UINT32 seed = 131; // 31 131 1313 13131 131313 etc..
   UINT32 hash = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash = (hash * seed) + pBuf[i];
   }

   return hash;
}
/* End Of BKDR Hash Function */


//////////////////////////////////////////////////////////////////////
// SDBM Hash Function
// This is the algorithm of choice which is used in the open source SDBM project.
// The hash function seems to have a good over-all distribution for many different data sets.
// It seems to work well in situations where there is a high variance in the MSBs of the elements in a data set. 
UINT32 i3Hash::SDBM( char * pBuf, INT32 len)
{
   UINT32 hash = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash = pBuf[i] + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}
/* End Of SDBM Hash Function */


//////////////////////////////////////////////////////////////////////
// DJB Hash Function
// An algorithm produced by Professor Daniel J. Bernstein and shown first to the world
// on the usenet newsgroup comp.lang.c. 
// It is one of the most efficient hash functions ever published. 

UINT32 i3Hash::DJB( char * pBuf, INT32 len)
{
   UINT32 hash = 5381;

   for( INT32 i = 0; i < len; i++)
   {
      hash = ((hash << 5) + hash) + pBuf[i];
   }

   return hash;
}
/* End Of DJB Hash Function */


///////////////////////////////////////////////////////////////////////
// DEK Hash Function
// An algorithm proposed by Donald E. Knuth in The Art Of Computer Programming Volume 3,
// under the topic of sorting and search chapter 6.4. 
UINT32 i3Hash::DEK( char * pBuf, INT32 len)
{
   UINT32 hash = (UINT32) len;

   for( INT32 i = 0; i < len; i++)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ pBuf[i];
   }

   return hash;
}
/* End Of DEK Hash Function */


UINT32 i3Hash::BP( char * pBuf, INT32 len)
{
   UINT32 hash = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash = hash << 7 ^ pBuf[i];
   }

   return hash;
}
/* End Of BP Hash Function */


UINT32 i3Hash::FNV( char * pBuf, INT32 len)
{
   const UINT32 fnv_prime = 0x811C9DC5;

   UINT32 hash = 0;

   for( INT32 i = 0; i < len; i++)
   {
      hash *= fnv_prime;
      hash ^= pBuf[i];
   }

   return hash;
}
/* End Of FNV Hash Function */


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

UINT32 i3Hash::AP( char * pBuf, INT32 len)
{
   UINT32 hash = 0xAAAAAAAA;

   for( INT32 i = 0; i < len; i++)
   {
      hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ pBuf[i] ^ (hash >> 3)) :
                               (~((hash << 11) ^ pBuf[i] ^ (hash >> 5)));
   }

   return hash;
}
/* End Of AP Hash Function */
