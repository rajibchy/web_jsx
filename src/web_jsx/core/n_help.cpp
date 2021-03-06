/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#	include "n_help.h"
#	include "web_jsx_global.h"
#	include "std_wrapper.hpp"
using namespace sow_web_jsx;

void n_help::add_http_status(std::vector<std::string>&http_status, std::string&values) {
	if (!http_status.empty()) {
		http_status.clear();
	}
	http_status.push_back(values);
}
response_status n_help::get_http_response_status(std::vector<std::string>& http_status) {
	for (size_t i = 0, len = http_status.size(); i < len; ++i) {
		std::string& str = http_status[i];
		if (str.find("200") != std::string::npos)return response_status::OK;
		if (str.find("301") != std::string::npos)return response_status::MOVED;
		if (str.find("302") != std::string::npos)return response_status::REDIRECT;
		if (str.find("303") != std::string::npos)return response_status::SEEOTHER;
		if (str.find("304") != std::string::npos)return response_status::NOTMODIFIED;
		if (str.find("401") != std::string::npos)return response_status::UNAUTHORIZED;
		if (str.find("403") != std::string::npos)return response_status::FORBIDDEN;
		if (str.find("404") != std::string::npos)return response_status::NOT_FOUND;
		if (str.find("500") != std::string::npos)return response_status::INTERNAL_SERVER_ERROR;
		if (str.find("501") != std::string::npos)return response_status::NOT_IMPLEMENTED;
	}
	return response_status::OK;
}
response_status n_help::get_http_response_status(int status_code) {
	if (status_code == 200)return response_status::OK;
	if (status_code == 301)return response_status::MOVED;
	if (status_code == 302)return response_status::REDIRECT;
	if (status_code == 303)return response_status::SEEOTHER;
	if (status_code == 304)return response_status::NOTMODIFIED;
	if (status_code == 401)return response_status::UNAUTHORIZED;
	if (status_code == 403)return response_status::FORBIDDEN;
	if (status_code == 404)return response_status::NOT_FOUND;
	if (status_code == 500)return response_status::INTERNAL_SERVER_ERROR;
	if (status_code == 501)return response_status::NOT_IMPLEMENTED;
	return response_status::OK;
}
int n_help::write_http_status(
	wjsx_env&wj_env, 
	bool check_status = false
) {
	int rec = TRUE;
	std::vector<std::string>& http_status = wj_env.get_http_status();
	for (size_t i = 0; i < http_status.size(); ++i) {
		if (rec == TRUE) {
			std::string&str = http_status[i];
			if (str.find("401") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("403") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("404") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("501") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("301") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("304") != std::string::npos) {
				rec = FALSE;
			}
			else if (str.find("303") != std::string::npos) {
				rec = FALSE;
			}
		}
		if (!check_status) {
			wj_env << http_status[i] << H_N_L;
		}
	}
	return rec;
}
int n_help::is_binary_response(std::map<std::string, std::string>& header) {
	if (header.empty())return FALSE;
	int rec = FALSE;
	
	auto it = header.find("Content-Transfer-Encoding");
	if (it != header.end()) {
		if (strings_equal(it->second, "binary"))return TRUE;
		/*if (it->second == "binary")return 1;*/
	}
	it = header.find("Accept-Ranges");
	if (it != header.end()) {
		if (strings_equal(it->second, "bytes"))return TRUE;
		/*if (it->second == "bytes")return 1;*/
	}
	return rec;
}
int n_help::is_gzip_encoding(std::map<std::string, std::string>&header) {
	if (header.empty())return FALSE;
	int rec = FALSE;
	auto it = header.find("Content-Encoding");
	if (it != header.end()) {
		if (strings_equal(it->second, "gzip") || strings_equal(it->second, "deflate"))return TRUE;
	}
	return rec;
}
int n_help::is_attachment_response(std::map<std::string, std::string>&header) {
	if (header.empty())return FALSE;
	int rec = FALSE;
	auto it = header.find("x-content-type");
	if (it != header.end()) {
		//if (it->second == "attachment")return 1;
		if (strings_equal(it->second, "attachment"))return TRUE;
	}
	return rec;
}
void n_help::write_cookies(wjsx_env&wj_env) {
	std::vector<std::string>& cookies = wj_env.get_http_cookies();
	if (cookies.empty())return;
	std::ostream& cout = wj_env.cout();
	for (size_t i = 0; i < cookies.size(); ++i) {
		cout << cookies[i] << H_N_L;
	}
}
void n_help::v8_object_loop(v8::Isolate* isolate, const v8::Local<v8::Object>v8_obj, std::map<const char*, const char*>&out_put) {
	v8::Local<v8::Context>ctx = isolate->GetCurrentContext();
	v8::Local<v8::Array> property_names = v8_obj->GetOwnPropertyNames(ctx).ToLocalChecked();
	uint32_t length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		v8::Local<v8::Value> key = property_names->Get(ctx, i).ToLocalChecked();
		v8::Local<v8::Value> value = v8_obj->Get(ctx, key).ToLocalChecked();
		if (value->IsNullOrUndefined())continue;
		if (key->IsString() && value->IsString()) {
			/*native_string native_key(isolate, key);
			native_string native_value(isolate, value);
			out_put[native_value.get_string().data()] = native_value.get_string().data();*/
			v8::String::Utf8Value utf8_key(isolate, key);
			v8::String::Utf8Value utf8_value(isolate, value);
			std::string* key_str = new std::string(*utf8_key);
			std::string*  val_str = new std::string(*utf8_value);
			out_put[key_str->data()] = val_str->data();
			//free(key_str); free(val_str);
		}
	}
}
#define H_PROCESS_BY "X-Process-By"
#define H_PROCESS_BY_VAL "WebJsx"
#define H_POWERED_BY "X-Powered-By"
#define H_POWERED_BY_VAL "safeonline.world"
//void  to_upper_case(std::string& str) {
//	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
//}
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
void n_help::add_header(std::map<std::string, std::string>& header, const char* key, const char* values) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase(it);
	}
	header[key] = values;
}
void n_help::remove_header(std::map<std::string, std::string>& header, const char* key) {
	auto it = header.find(key);
	if (it != header.end()) {
		header.erase(it);
	}
}
const char* get_server_error(response_status status_code) {
	switch (status_code)
	{
	case response_status::UNAUTHORIZED:return "Unauthorized";
	case response_status::FORBIDDEN:return "Forbidden";
	case response_status::NOT_FOUND:return "Not Found";
	case response_status::INTERNAL_SERVER_ERROR: return "Internal Server Error";
	case response_status::NOT_IMPLEMENTED:return "Not Implemented";
	default:
		return "Server Error";
	}
}
void n_help::error_response(
	const char* server_root,
	response_status status_code,
	const char* error_msg,
	wjsx_env& wj_env
) {
	std::ostream& cout = wj_env.cout();
	cout << "Content-Type:text/html" << H_N_L;
	cout << "Accept-Ranges:bytes" << H_N_L;
	int status = (int)status_code;
	cout << "Status:" << status << " " << get_server_error(status_code) << H_N_L;
	cout << H_PROCESS_BY << ":" << H_PROCESS_BY_VAL << H_N_L;
	cout << H_POWERED_BY << ":" << H_POWERED_BY_VAL << H_N_L;
	cout << "WebJsx-Error-Code:" << status << H_N_L;
	cout << "\r\n";
	std::string* str = new std::string(server_root);
	str->append("error\\");
	str->append(std::to_string(status));
	str->append(".html");
	std::string* body = new std::string();
	int ret = ::read_file(str->c_str(), *body);
	if (::is_error_code(ret) == TRUE) {
		std::string html_body = REGEX_REPLACE_MATCH(*body, std::regex("(<%(.+?)%>)"), [&error_msg](const std::smatch& m) {
			return error_msg;
		});
		cout << html_body;
		html_body.clear(); std::string().swap(html_body);
	}
	else {
		cout << "No Error file found in /error/" << status << ".html<br/>";
		cout << "Server root:" << server_root << "<br/><br/><br/>";
		cout << error_msg;
	}
	_free_obj(body); _free_obj(str);
	cout.flush();
}
void n_help::write_header(wjsx_env&wj_env) {
	std::map<std::string, std::string>& headers = wj_env.get_http_header();
	if (headers.empty())return;
	std::string po_by(H_PROCESS_BY);
	std::string pow_by(H_POWERED_BY);
	std::ostream& cout = wj_env.cout();
	for (auto it = headers.begin(); it != headers.end(); ++it) {
		if (strings_equal(it->first, po_by))continue;
		if (strings_equal(it->first, pow_by))continue;
		cout << it->first << ":" << it->second << H_N_L;
	}
	po_by.clear(); po_by.clear();
	cout << H_PROCESS_BY << ":" << H_PROCESS_BY_VAL << H_N_L;
	cout << H_POWERED_BY << ":" << H_POWERED_BY_VAL << H_N_L;
	return;
}
//https://stackoverflow.com/questions/3381614/c-convert-string-to-hexadecimal-and-vice-versa
void sow_web_jsx::string_to_hex(const std::string& input, std::string& output){
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return;
}
void sow_web_jsx::hex_to_string(const std::string& input, std::string& output) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	if (len & 1) throw std::invalid_argument("odd length");
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2) {
		char a = input[i];
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) throw std::invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) throw std::invalid_argument("not a hex digit");
		output.push_back((char)((p - lut) << 4) | (char)(q - lut));
	}
	return;
}
