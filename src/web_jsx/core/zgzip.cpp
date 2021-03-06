/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "zgzip.hpp"
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
//3:45 PM 11/24/2018
#	include	<iostream>
#	include	<stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#	include	<string>// !_XSTRING_// memcpy, memset
#if (defined(_WIN32)||defined(_WIN64))
#	include	<windows.h>
#	include	<fcntl.h>
#	include	<io.h>
#if !defined(OS_CODE)
#	define OS_CODE  0x00
#endif//!OS_CODE
#else
#if !defined(OS_CODE)
#	define OS_CODE  0x03 //Assume Unix
#endif//!OS_CODE
#endif//_WIN32
#	include	<sstream> // std::stringstream
#	include	<fstream>// std::ifstream
#	include	<zlib.h>
#if !defined(CHUNK)
#	define CHUNK 16384
#endif//!CHUNK
/*
0x00	FAT filesystem (MS-DOS, OS/2, NT/Win32)
0x01	Amiga
0x02	VMS (or OpenVMS)
0x03	Unix
0x04	VM/CMS
0x05	Atari TOS
0x06	HPFS filesystem (OS/2, NT)
0x07	Macintosh
0x08	Z-System
0x09	CP/M
0x0a	TOPS-20
0x0b	NTFS filesystem (NT)
0x0c	QDOS
0x0d	Acorn RISCOS
0xff	unknown
*/
#if MAX_MEM_LEVEL >= 8
#	define DEF_MEM_LEVEL 8
#else
#	define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif//!MAX_MEM_LEVEL
#if !defined(assert)
#	define assert(expression) ((void)0)
#endif//!assert
//Read more http://www.zlib.net/zpipe.c
#if !defined(_free_zstream)
#	define _free_zstream(strm)\
while(strm){\
	delete strm;strm = NULL;\
}
#endif//!_free_zstream
#	define	GZIP_MAGIC     "\037\213"	/* Magic header for gzip files, 1F 8B */
//Success run on 2:05 AM 1/19/2019
//https://stackoverflow.com/questions/54256829/zlib-gzip-invalid-response-defined-in-web-browser-c
template<class _out_stream>
void write_magic_header(_out_stream& output) {
	size_t len = 10;
	char* dest = new char[len];
	//sprintf_s(dest, len, "%c%c%c%c%c%c%c%c%c%c", GZIP_MAGIC[0], GZIP_MAGIC[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
	sprintf(dest, "%c%c%c%c%c%c%c%c%c%c", GZIP_MAGIC[0], GZIP_MAGIC[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
	output.write(dest, len);
	delete[]dest;
}
static z_stream* create_z_stream() {
	z_stream* _strm = new z_stream();
	/* allocate deflate state */
	_strm->zalloc = (alloc_func)0;
	_strm->zfree = (free_func)0;
	_strm->opaque = (voidpf)0;
	return _strm;
}
template<class _stream>
size_t get_size_of_stream(_stream& _strm) {
	_strm.seekg(0, std::ios::end);//Go to end of stream
	std::streamoff length = _strm.tellg();
	_strm.seekg(0, std::ios::beg);//Back to begain of stream
	return static_cast<size_t>(length);
}
template<class _out_stream>
int deflate_stream(std::stringstream& source, _out_stream& dest, int level = Z_BEST_SPEED) {
	//6:08 AM 1/17/2019
	z_stream* strm = create_z_stream();
	/* negative windowBits to deflateInit2_ means "no header" */
	int ret = deflateInit2_(strm, level, Z_DEFLATED,
		-MAX_WBITS,
		DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		ZLIB_VERSION, (int)sizeof(z_stream));
	if (ret != Z_OK) {
		_free_zstream(strm);
		return ret;
	}
	size_t totalSize = get_size_of_stream(source);
	std::streamoff utotalSize = totalSize;
	int write_len = 0, flush;
	std::streamsize n;
	/* compress until end of stream */
	uLong tcrc = 0;
	do {
		char* in = new char[CHUNK];
		n = source.rdbuf()->sgetn(in, CHUNK);
		strm->avail_in = (uInt)n;
		tcrc = crc32(tcrc, (uint8_t*)in, (uInt)n);
		totalSize -= n;
		flush = totalSize == 0 || totalSize == std::string::npos ? Z_FINISH : Z_NO_FLUSH;
		strm->next_in = (Bytef*)in;
		/* run deflate() on input until output buffer not full, finish
		  compression if all of source has been read in */
		do {
			char* out = new char[CHUNK];
			strm->avail_out = CHUNK;
			strm->next_out = (Bytef*)out;
			ret = deflate(strm, flush);    /* no bad return value */
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			unsigned have = CHUNK - strm->avail_out;
			dest.write(out, have);
			write_len += have;
			delete[]out;
		} while (strm->avail_out == 0);
		assert(strm->avail_in == 0);     /* all input will be used */
		 /* Free memory */
		delete[]in;
	} while (flush != Z_FINISH);
	assert(ret == Z_STREAM_END);        /* stream will be complete */
	 /* clean up and return */
	(void)deflateEnd(strm);
	_free_zstream(strm);
	/* write gzip footer to out stream*/
	dest.write((char*)&tcrc, sizeof(tcrc));
	dest.write((char*)&utotalSize, sizeof(utotalSize));
	return write_len;
}
void gzip::compress_gzip(std::stringstream& source_stream, std::ostream& out_stream){
	write_magic_header(out_stream);
	deflate_stream(source_stream, out_stream);
}
