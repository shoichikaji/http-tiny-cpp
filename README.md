# http tiny cpp

WIP

# Usage

```cpp
#include <http-tiny.hpp>

HTTPTinyResponse res = HTTPTiny().get("http://example.com/");

if (!res.success) {
  cerr << "Failed!" << endl;
  exit(1);
}

cout << res.status << " " << res.reason << endl;

for (map<string, string>::const_iterator it = res.headers.begin();
  it != res.headers.end(); ++it) {
  cout << it->first << ": " << it->second << endl;
}

if (res.content.size() > 0)
  cout << res.content;
```

# See Also

* https://metacpan.org/pod/HTTP::Tiny
* https://github.com/mrtazz/restclient-cpp
* http://curl.haxx.se/libcurl/

# Author

Shoichi Kaji

# License

MIT
