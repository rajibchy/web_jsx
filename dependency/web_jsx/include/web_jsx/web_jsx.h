/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//9:49 PM 1/14/2020
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_web_jsx_h)
#	define _web_jsx_h
#	include <v8.h>
#	include <io.h> 
#	include <regex>
#	include <sstream>

#if !defined(FAST_CGI_APP)
#	define FAST_CGI_APP 1
#endif//!FAST_CGI_APP

#if !defined(NULL)
    #if defined(__cplusplus)
        #	define NULL 0
    #else
        #	define NULL ((void *)0)
    #endif//!__cplusplus
#endif//!NULL

#if !defined(WJSX_API)
#	define WJSX_API(type) type
#endif//!WJSX_API

#if !defined(EXPORT_WJSX)
#if (defined(_WIN32)||defined(_WIN64))
#	define EXPORT_WJSX __declspec(dllexport)
#else
#	define EXPORT_WJSX
#endif//_WIN32||_WIN64
#endif//!EXPORT_WJSX

#if !defined(__file_exists)
#define __file_exists(fname)\
_access(fname, 0)!=-1
#endif//!__file_exists

#if !defined(TYPE_CHECK)
#define TYPE_CHECK(T, S)                                       \
  while (false) {                                              \
    *(static_cast<T* volatile*>(0)) = static_cast<S*>(0);      \
  }
#endif//!TYPE_CHECK

#	include <web_jsx/wjsx_env.h>

#if !defined(v8_str)
#define v8_str(isolate, x)\
	v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked()
#endif//!v8_str

#if !defined(throw_js_error)
#define throw_js_error(isolate, err)\
	isolate->ThrowException(v8::Exception::Error(v8_str(isolate, err)))
#endif//!throw_js_error

#if !defined(throw_js_type_error)
#define throw_js_type_error(isolate, err)\
	isolate->ThrowException(v8::Exception::TypeError(v8_str(isolate, err)))
#endif//!throw_js_type_error

#if !defined(js_method_args)
#define js_method_args const v8::FunctionCallbackInfo<v8::Value>& args
#endif//!js_method_args

#if !defined(V8_JS_METHOD)
#define V8_JS_METHOD(name)\
void name(js_method_args)
#endif//!V8_JS_METHOD

#if !defined(set_prototype)
#define set_prototype(isolate, prototype, name, func)\
prototype->Set(isolate, name, v8::FunctionTemplate::New(isolate, func))
#endif//!set_prototype

//target->Set(isolate->GetCurrentContext(), v8_str(isolate, name), obj )
#if !defined(wjsx_set_object)
#define wjsx_set_object(isolate, target, name, obj)\
	target->DefineOwnProperty(\
		isolate->GetCurrentContext(),\
		v8_str(isolate, name),\
		obj,\
		v8::PropertyAttribute::ReadOnly\
	)
#endif//!register_wjsx_module

//target->Set(isolate->GetCurrentContext(), v8_str(isolate, name), v8::Function::New(isolate->GetCurrentContext(), func).ToLocalChecked() )
#if !defined(wjsx_set_method)
#if V8_MAJOR_VERSION < 8
#define wjsx_set_method(isolate, target, name, func)\
	target->Set(isolate->GetCurrentContext(), v8_str(isolate, name), v8::Function::New(isolate, func) )
#else
#define wjsx_set_method(isolate, target, name, func)\
	wjsx_set_object(isolate, target, name, v8::Function::New(isolate->GetCurrentContext(), func).ToLocalChecked())
#endif//!V8_MAJOR_VERSION
#endif//!wjsx_set_method

#if !defined(to_char_str_n)
#define to_char_str_n(value)\
	value.length() <= 0 ? "" :(*value ? *value : "<string conversion failed>")
#endif//!to_char_str_n

#if !defined(_free_obj)
#	define _free_obj(obj)\
while(obj){\
	obj->clear();delete obj;obj = NULL;\
}
#endif//!_free_obj

#if !defined(_free_char)
#	define _free_char(obj)\
while(obj){\
	delete[] obj; obj = NULL;\
}
#endif//!_free_char

#if !defined(FALSE)
#	define FALSE               0
#endif//!FALSE

#if !defined(TRUE)
#	define TRUE                1
#endif//!FALSE

__forceinline static void format__path(std::string& str) {
	str = std::regex_replace(str, std::regex("(?:/)"), "\\");
}
__forceinline void get_dir_from_path (const std::string& path_str, std::string&dir) {
	size_t found = path_str.find_last_of("/\\");
	dir = path_str.substr(0, found);
}
__forceinline static const char* to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
	v8::String::Utf8Value str(isolate, x);
	return to_char_str_n(str);
}

template<class _input>
inline int is_error_code(_input ret) {
	return (ret == FALSE || ret == std::string::npos || ret < 0) ? TRUE : FALSE;
}
extern "C" {
	EXPORT_WJSX void web_jsx_native_module(v8::Handle<v8::Object>target);
}
/*[function pointers]*/
typedef void (*add_resource_func)();
/*[/function pointers]*/
namespace sow_web_jsx {
	WJSX_API(void) register_resource(add_resource_func func);
	namespace wrapper {
		WJSX_API(void) clear_cache(wjsx_env& wj_env);
		WJSX_API(void) add_header(wjsx_env* wj_env, const char* key, const char* value);
		WJSX_API(int) is_http_status_ok(wjsx_env* wj_env);
		WJSX_API(int) is_gzip_encoding(wjsx_env* wj_env);
		WJSX_API(int) flush_http_status(wjsx_env* wj_env);
		WJSX_API(void) flush_header(wjsx_env* wj_env);
		WJSX_API(void) flush_cookies(wjsx_env* wj_env);
	}
}

#endif//!_web_jsx_h