#ifndef HTTPTINY_H_
#define HTTPTINY_H_

#include <string>
#include <map>
#include <vector>
#include <curl/curl.h>

struct HTTPTinyResponse {
  bool success;
  unsigned int status;
  std::string reason;
  std::string protocol;
  std::string url;
  std::string content;
  std::map<std::string, std::string> headers;
  HTTPTinyResponse();
  void clear();
};

struct HTTPTinyOption {
  std::map<std::string, std::string> headers;
  std::string content;
  HTTPTinyOption();
};

class HTTPTiny {
public:
  std::string agent;
  std::map<std::string, std::string> default_headers;
  unsigned int max_redirect;
  unsigned int timeout;
  bool verify_ssl;

  HTTPTiny();
  ~HTTPTiny();
  HTTPTinyResponse request(const std::string& method,
                           const std::string& url,
                           const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse get(const std::string& url, const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse post(const std::string& url, const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse put(const std::string& url, const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse del(const std::string& url, const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse mirror(const std::string& url,
                          const std::string& filename,
                          const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse post_form(const std::string& url,
                             const std::map<std::string, std::string>& form_data,
                             const HTTPTinyOption& option = HTTPTinyOption()) const;
  HTTPTinyResponse post_form(const std::string& url,
                             const std::vector<std::string>& form_data,
                             const HTTPTinyOption& option = HTTPTinyOption()) const;
private:
  CURL *curl;
};

#endif
