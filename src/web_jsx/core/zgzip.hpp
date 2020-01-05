/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_zgzip_h)
#define _zgzip_h
#pragma warning (disable : 4231)
#pragma warning(disable : 4996)
//3:45 PM 11/24/2018
#if !(defined(_WIN32)||defined(_WIN64)) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#error Have to check !TODO
#else
#if !defined(_IOSTREAM_)
#include <iostream>
#endif//!_IOSTREAM_
#if !defined(_WINDOWS_)
#include <windows.h>
#endif//!_WINDOWS_
#endif//_WIN32||_WIN64/__unix__
#if !defined(_INC_STDIO)
#include <stdio.h>  /* defines FILENAME_MAX, printf, sprintf */
#endif//!_INC_STDIO
#if !defined(_XSTRING_)
#include <string>// !_XSTRING_// memcpy, memset
#endif //!_XSTRING_
#if !defined(ZLIB_H)
#include <zlib.h>
#endif//!ZLIB_H
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#if !defined(_INC_FCNTL)
#  include <fcntl.h>
#endif//_INC_FCNTL
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#if defined(__CYGWIN__)
#define SET_BINARY_MODE(file) setmode(fileno(my_stdio_stream), O_BINARY)
#elif defined(_WIN32) || defined(MSDOS) || defined(OS2)
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), _O_BINARY)
#endif//!__CYGWIN__
#endif//!SET_BINARY_MODE
#else
#if !defined(_INC_IO)
#  include <io.h>
#endif//!_INC_IO
#if !defined(SET_BINARY_MODE)
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#endif//!SET_BINARY_MODE
#endif//WIN32
#if !defined(_SSTREAM_)
#include <sstream> // std::stringstream
#endif//_SSTREAM_
#if !defined(CHUNK)
#define CHUNK 16384
#endif//!CHUNK
#if !defined(OS_CODE)
#  define OS_CODE  0x03
#endif//!OS_CODE
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif//!MAX_MEM_LEVEL
#if !defined(assert)
#define assert(expression) ((void)0)
#endif//!assert
namespace gzip {
	//Success run on 2:05 AM 1/19/2019
	//https://stackoverflow.com/questions/54256829/zlib-gzip-invalid-response-defined-in-web-browser-c
	static int gz_magic[2] = { 0x1f, 0x8b }; /* gzip magic header */
	template<class _out_stream>
	void write_magic_header(_out_stream&output) {
		char* dest = new char[10];
		sprintf(dest, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1], Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
		output.write(const_cast<const char*>(dest), 10);
		delete[]dest;
	}
	template<class _out_stream>
	int deflate_stream(std::stringstream&source, _out_stream&dest, int level = Z_BEST_SPEED) {
		//6:08 AM 1/17/2019
		int ret, flush;
		unsigned have;
		z_stream strm;
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit2_(&strm, level, Z_DEFLATED,
			-MAX_WBITS,
			DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			ZLIB_VERSION, (int)sizeof(z_stream));
		if (ret != Z_OK)
			return ret;
		source.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = source.tellg();
		std::streamoff utotalSize = totalSize;
		source.seekg(0, std::ios::beg);//Back to begain of stream
		int write_len = 0;
		//bool is_first = true;
		std::streamsize n;
		/* compress until end of stream */
		uLong tcrc = 0;
		do {
			char* in = new char[CHUNK];
			n = source.rdbuf()->sgetn(in, CHUNK);
			strm.avail_in = (uInt)n;
			tcrc = crc32(tcrc, (uint8_t*)in, (uInt)n);
			totalSize -= n;
			flush = totalSize <= 0 ? Z_FINISH : Z_NO_FLUSH;
			strm.next_in = (Bytef*)in;
			/* run deflate() on input until output buffer not full, finish
			  compression if all of source has been read in */
			do {
				char* out = new char[CHUNK];
				strm.avail_out = CHUNK;
				strm.next_out = (Bytef*)out;
				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = CHUNK - strm.avail_out;
				dest.write(out, have);
				write_len += have;
				delete[]out;
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */
			 /* done when last data in file processed */
			delete[]in;
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */
		 /* clean up and return */
		(void)deflateEnd(&strm);
		/* write gzip footer to out stream*/
		dest.write((char*)&tcrc, sizeof(tcrc));
		dest.write((char*)&utotalSize, sizeof(utotalSize));
		//std::ifstream ifs;
		return write_len;
	}
	template<class _out_stream>
	void compress_gzip (std::stringstream&source_stream, _out_stream&out_stream) {
		write_magic_header(out_stream);
		deflate_stream(source_stream, out_stream);
	}
	//2:28 PM 1/4/2020
	template<class _out_stream>
	class gzip_deflate {
	public:
		gzip_deflate(_out_stream& dest, int level);
		~gzip_deflate();
		size_t write(const char* buff, int do_flush);
		size_t write(std::stringstream& source, int do_flush);
		size_t write(const std::string file_path, int do_flush);
		int flush();
		int has_error();
		const char* get_last_error();
	private:
		_out_stream* _dest;
		z_stream _strm;
		uLong _tcrc;
		int _stream_flush;
		int _is_flush;
		long _total_size;
		int _is_error;
		char* _internal_error;
		size_t write(char* in, size_t len, int do_flush, int bypass);
		int panic(const char* error, int error_code);
	};
	template<class _out_stream>
	inline gzip_deflate<_out_stream>::gzip_deflate(_out_stream& dest, int level){
		_is_flush = FALSE; _total_size = NULL; _tcrc = NULL; _is_error = FALSE;
		if (level == FALSE)level = Z_BEST_SPEED;
		_internal_error = new char;
		write_magic_header(dest);
		_strm.zalloc = Z_NULL;
		_strm.zfree = Z_NULL;
		_strm.opaque = Z_NULL;
		int ret = deflateInit2_(&_strm, level, Z_DEFLATED,
			-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			ZLIB_VERSION, (int)sizeof(z_stream)
		);
		if (ret != Z_OK) {
			panic("Unable to initilize deflateInit2_", TRUE);
			return;
		}
		_stream_flush = Z_NO_FLUSH;
		_dest = &dest;
	}
	template<class _out_stream>
	inline gzip_deflate<_out_stream>::~gzip_deflate(){
		if (_is_flush == TRUE)return;
		this->flush();
		if (_internal_error != NULL) {
			delete[]_internal_error;
			_internal_error = NULL;
		}
	}
	template<class _out_stream>
	inline size_t gzip_deflate<_out_stream>::write(char* in, size_t len, int do_flush, int bypass) {
		if (bypass == FALSE) {
			if (_is_flush == TRUE)return FALSE;
			if (_is_error == TRUE)return -1;
			if (_stream_flush == Z_FINISH) {
				delete[]in;
				return panic("deflate::state Z_FINISH", -1);
			}
			if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
				delete[]in;
				return panic("deflate::Invalid stream end request.", -1);
			}
			_is_error = FALSE;
		}
		int ret; _stream_flush = do_flush;
		unsigned have;
		_strm.avail_in = (uInt)len;
		_tcrc = crc32(_tcrc, (uint8_t*)in, (uInt)len);
		_strm.next_in = (Bytef*)in;
		/* run deflate() on input until output buffer not full, finish
		  compression if all of source has been read in */
		do {
			char* out = new char[CHUNK];
			_strm.avail_out = CHUNK;
			_strm.next_out = (Bytef*)out;
			ret = deflate(&_strm, _stream_flush);    /* no bad return value */
			if (ret == Z_STREAM_ERROR) {
				delete[]in;
				return panic("deflate::state not clobbered", TRUE);
			}
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = CHUNK - _strm.avail_out;
			_dest->write(out, have);
			_total_size += have;
			delete[]out;
		} while (_strm.avail_out == 0);
		if (_strm.avail_in != 0) {
			delete[]in;
			return panic("deflate::all input unable to use", TRUE);
		}
		//assert(_strm.avail_in == 0);     /* all input will be used */
		if (ret != Z_STREAM_END) {
			delete[]in;
			return panic("deflate::stream not completed yet", TRUE);
		}
		//assert(ret == Z_STREAM_END);        /* stream will be complete */
		 /* done when last data in file processed */
		delete[]in;
		return len;
	}
	template<class _out_stream>
	inline size_t gzip_deflate<_out_stream>::write(const char* buff, int do_flush){
		//Z_FINISH : Z_NO_FLUSH
		return this->write(strdup(buff), strlen(buff), do_flush, FALSE);
	}
	template<class _out_stream>
	inline size_t gzip_deflate<_out_stream>::write(std::stringstream& source, int do_flush){
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_stream_flush == Z_FINISH) {
			return panic("deflate::state Z_FINISH", -1);
		}
		if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
			return panic("deflate::Invalid stream end request.", -1);
		}
		source.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = source.tellg();
		size_t total_len = (size_t)totalSize;
		source.seekg(0, std::ios::beg);//Back to begain of stream
		int end_flush = do_flush == Z_FINISH ? Z_FINISH : Z_NO_FLUSH;
		do_flush = Z_NO_FLUSH;
		size_t ret; size_t read_len = 0;
		//std::streamsize n;
		do {
			char* in;
			if (totalSize > CHUNK) {
				read_len = CHUNK;
			}
			else {
				read_len = totalSize;
			}
			in = new char[read_len];
			source.rdbuf()->sgetn(in, read_len);
			totalSize -= read_len;
			if (totalSize <= 0) {
				ret = this->write(in, read_len, end_flush, TRUE);
				if (ret == FALSE || ret == std::string::npos || ret < 0)return ret;
				break;
			}
			ret = this->write(in, read_len, do_flush, TRUE);
			if (ret == FALSE || ret == std::string::npos || ret < 0)return ret;
		} while (true);
		return total_len;
	}
	template<class _out_stream>
	inline size_t gzip_deflate<_out_stream>::write(const std::string file_path, int do_flush){
		if (_is_flush == TRUE)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_stream_flush == Z_FINISH) {
			return panic("deflate::state Z_FINISH", -1);
		}
		if (!(do_flush == Z_FINISH || do_flush == Z_NO_FLUSH)) {
			return panic("deflate::Invalid stream end request.", -1);
		}
		std::ifstream file(file_path.c_str(), std::ifstream::binary);
		if (!file.is_open()) {
			return panic("Unable to open file....", -1);
		}
		file.seekg(0, std::ios::end);//Go to end of stream
		std::streamoff totalSize = file.tellg();
		size_t total_len = (size_t)totalSize;
		file.seekg(0, std::ios::beg);//Back to begain of stream
		int end_flush = do_flush == Z_FINISH ? Z_FINISH : Z_NO_FLUSH;
		do_flush = Z_NO_FLUSH;
		size_t ret; size_t read_len = 0;
		//std::streamsize n;
		do {
			char* in;
			if (totalSize > CHUNK) {
				read_len = CHUNK;
			}
			else {
				read_len = totalSize;
			}
			in = new char[read_len];
			file.read(in, read_len);
			totalSize -= read_len;
			if (totalSize <= 0) {
				ret = this->write(in, read_len, end_flush, TRUE);
				if (ret == FALSE || ret == std::string::npos || ret < 0) {
					file.close();
					return ret;
				}
				break;
			}
			ret = this->write(in, read_len, do_flush, TRUE);
			if (ret == FALSE || ret == std::string::npos || ret < 0) {
				file.close();
				return ret;
			}
		} while (true);
		file.close();
		return total_len;
	}
	template<class _out_stream>
	inline int gzip_deflate<_out_stream>::flush(){
		if (_is_flush)return FALSE;
		if (_is_error == TRUE)return -1;
		if (_stream_flush != Z_FINISH) {
			return panic("deflate::state yet not Z_FINISH", -1);
		}
		/* clean up and return */
		(void)deflateEnd(&_strm);
		/* write gzip footer to out stream*/
		_dest->write((char*)& _tcrc, sizeof(_tcrc));
		_dest->write((char*)& _total_size, sizeof(_total_size));
		_dest = NULL; _is_flush = TRUE;
		return TRUE;
	}
	template<class _out_stream>
	inline int gzip_deflate<_out_stream>::has_error(){
		return _is_error == TRUE || _is_error < 0 ? TRUE : FALSE;
	}
	template<class _out_stream>
	inline const char* gzip_deflate<_out_stream>::get_last_error() {
		if (_is_error == TRUE || _is_error < 0) {
			return const_cast<const char*>(_internal_error);
		}
		return "No Error Found!!!";
		
	}
	template<class _out_stream>
	inline int gzip_deflate<_out_stream>::panic(const char* error, int error_code){
		if (_internal_error != NULL)
			delete[]_internal_error;
		_internal_error = new char[strlen(error) + 1];
		strcpy(_internal_error, error);
		_is_error = error_code;
		return _is_error;
	}
}; // namespace gzip
#endif//_zgzip_h
