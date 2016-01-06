#include <string>
#include <iostream>
#include "http-tiny.hpp"
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: main URL\n";
    return 1;
  }

  const char* url = argv[1];

  HTTPTinyResponse res = HTTPTiny().request("GET", url);

  if (!res.success) {
    cerr << "Failed!" << endl;
    return 1;
  }

  cout << res.status << " " << res.reason << endl;

  for (map<string, string>::const_iterator it = res.headers.begin();
    it != res.headers.end(); ++it) {
    cout << it->first << ": " << it->second << endl;
  }

  if (res.content.size() > 0)
    cout << res.content;

  return 0;
}
