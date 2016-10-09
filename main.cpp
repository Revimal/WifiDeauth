/*The MIT License (MIT)
 *
 * Copyright (c) 2016 Revimal(Hyeonho Seo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

// I defined system headers and macros here.
// Because, 'wifi_deauth.h' and 'wifi_deauth.cpp' must be system independent
// module.
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/stat.h>
#include <sys/types.h>
#define NIC_LOCATION "/sys/class/net/"

#elif defined(__WIN32)
#endif

#include "wifi_deauth.h"
#include <cstdlib>

inline int chkWiFiChannel(int _channel) {
  // Not compatible to 5Ghz 802.11 Yet
  if (_channel < 1 || _channel > 14)
    return 1;
  return _channel;
}

bool isExistDirectory(const char *nic_name) {
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  struct stat sb;
  char path_name[100];

  snprintf(path_name, sizeof(path_name), "%s%s", NIC_LOCATION, nic_name);
  if (stat(path_name, &sb) == 0 && S_ISLNK(sb.st_mode) == 0) {
    return true;
  }

  return false;

#elif defined(__WIN32)

  return true;
#else
  // If unsupported environment, Not the network interface exist check.
  return true;
#endif
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cout << "[*] Usage : " << argv[0]
              << " [Monitor Interface] [Managed Interface] [WiFi Channel]"
              << std::endl;
    std::cout << "    If you make a wrong input to 'WiFi Channel', channel is "
                 "setted by 1."
              << std::endl;
    return -1;
  }

  if (isExistDirectory(argv[1]) == false ||
      isExistDirectory(argv[2]) == false) {
    std::cout << "[*] Input interface is not exist." << std::endl;
    std::cout << "    Please check your interface." << std::endl;
    return -1;
  }

  WifiDeauth deauth(argv[1], argv[2], chkWiFiChannel(atoi(argv[3])));
  deauth.runDeauth();

  return 0;
}
