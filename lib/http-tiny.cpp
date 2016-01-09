#include "http-tiny.hpp"
#include <cstdlib> // strtol
#include <sstream>

using namespace std;

HTTPTinyResponse::HTTPTinyResponse() :
  success(false), status(0), reason(),
  protocol(), url(), content(), headers() {}
void HTTPTinyResponse::clear() {
  success = false;
  status = 0;
  reason.clear();
  protocol.clear();
  url.clear();
  content.clear();
  headers.clear();
}
HTTPTinyOption::HTTPTinyOption() : headers(), content() {}

HTTPTiny::HTTPTiny() :
  agent("http-tiny-cpp/0.01"),
  default_headers(),
  max_redirect(5),
  timeout(60),
  verify_ssl(false),
  curl(NULL)
{
  this->curl = curl_easy_init();
  curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1);
}

HTTPTiny::~HTTPTiny() {
  if (curl) {
    curl_easy_cleanup(curl);
    curl = NULL;
  }
}

namespace {
// copy from https://github.com/mrtazz/restclient-cpp
inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}
// trim from end
inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}
// trim from both ends
inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

size_t recieve_header_callback(char *data, size_t size, size_t nmemb, void *userdata) {
  HTTPTinyResponse* res;
  res = reinterpret_cast<HTTPTinyResponse*>(userdata);
  string header(data, size * nmemb);
  size_t seperator = header.find_first_of(":");
  if (std::string::npos == seperator) {
    //roll with non seperated headers...
    trim(header);
    if (0 == header.length()) {
      return size * nmemb; //blank line;
    }
    // HTTP/1.X YYY
    // 1234567890123456... -> 12chars
    if (header.find_first_of("HTTP/1.") != 0
        || header.size() < 12
        || header.at(8) != ' '
    ) {
      return size * nmemb;
    }
    // this is request line!
    // before parsing request line, clear previous response
    res->clear();
    if (header.find_first_of("HTTP/1.1") == 0) {
      res->protocol = "HTTP/1.1";
    } else if (header.find_first_of("HTTP/1.0") == 0) {
      res->protocol = "HTTP/1.0";
    }
    res->status = strtol(header.substr(9, 3).c_str(), NULL, 10);
    if (header.size() >= 14) {
      res->reason = header.substr(13);
    }
  } else {
    std::string key = header.substr(0, seperator);
    trim(key);
    std::string value = header.substr(seperator + 1);
    trim(value);
    if (res->headers.find(key) != res->headers.end()) {
      res->headers[key] = res->headers[key] + ", " + value;
    } else {
      res->headers[key] = value;
    }
  }
  return size * nmemb;
}
size_t recieve_content_callback(char *data, size_t size, size_t nmemb, void *userdata) {
  HTTPTinyResponse* res;
  res = reinterpret_cast<HTTPTinyResponse*>(userdata);
  res->content.append(reinterpret_cast<char*>(data), size * nmemb);
  return size * nmemb;
}
};

HTTPTinyResponse HTTPTiny::get(const string& url,
                               const HTTPTinyOption& option) const {
  return this->request("GET", url, option);
}
HTTPTinyResponse HTTPTiny::post(const string& url,
                                const HTTPTinyOption& option) const {
  return this->request("POST", url, option);
}
HTTPTinyResponse HTTPTiny::put(const string& url,
                               const HTTPTinyOption& option) const {
  return this->request("PUT", url, option);
}
HTTPTinyResponse HTTPTiny::del(const string& url,
                               const HTTPTinyOption& option) const {
  return this->request("DELETE", url, option);
}

HTTPTinyResponse HTTPTiny::request(const string& method,
                                   const string& url,
                                   const HTTPTinyOption& option) const {
  HTTPTinyResponse res;
  curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, method.c_str());
  curl_easy_setopt(this->curl, CURLOPT_MAXREDIRS, this->max_redirect);
  curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, recieve_header_callback);
  curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &res);
  curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, recieve_content_callback);
  curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &res);
  curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, this->timeout);
  curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, this->verify_ssl ? 1 : 0);
  CURLcode curl_code = curl_easy_perform(this->curl);

  if (curl_code != CURLE_OK) {
    res.clear();
    res.status = 599;
    res.reason = "Internal Exception";
    string error = curl_easy_strerror(curl_code);
    ostringstream error_len;
    error_len << error.size();
    res.headers["Cotent-Length"] = error_len.str();
    res.headers["Content-Type"] = "text/plain";
    res.content = error;
  } else {
    int status = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &status);
    res.status = status;
    if (200 <= res.status && res.status <= 299) {
      res.success = true;
    }
  }
  char* last_url;
  curl_easy_getinfo(this->curl, CURLINFO_EFFECTIVE_URL, &last_url);
  if (last_url) {
    res.url = last_url;
  }
  return res;
}

HTTPTinyResponse HTTPTiny::mirror(const string& url,
                                  const string& filename,
                                  const HTTPTinyOption& option) const {
  return HTTPTinyResponse();
}

HTTPTinyResponse HTTPTiny::post_form(const string& url,
                                     const map<string, string>& form_data,
                                     const HTTPTinyOption& option) const {
  vector<string> vec;
  for (map<string, string>::const_iterator it = form_data.begin();
      it != form_data.end(); ++it) {
    vec.push_back(it->first);
    vec.push_back(it->second);
  }
  return this->post_form(url, vec, option);
}

HTTPTinyResponse HTTPTiny::post_form(const string& url,
                                     const vector<string>& form_data,
                                     const HTTPTinyOption& option) const {
  return HTTPTinyResponse();
}

