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
 * The above copyright notice and this permission notice shall be included in all
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

#include <stdlib.h>
#include "wifi_deauth.h"

#define NIC_LOCATION "/sys/class/net/"

bool isExistDirectory(const char* nic_name) {
  struct stat sb;
  char path_name[100];

  snprintf(path_name, sizeof(path_name), "%s%s", NIC_LOCATION, nic_name);
  if (stat(path_name, &sb) == 0 && S_ISLNK(sb.st_mode) == 0) {
    return true;
  }

  return false;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "[*] Usage : " << argv[0] << " [Monitor Interface] [Managed Interface] [WiFi Channel]" << std::endl;
    std::cout << "If you make a wrong input to 'WiFi Channel', channel is setted by 0." << std::endl;
    return -1;
  }

  if (isExistDirectory(argv[1]) == false || isExistDirectory(argv[2]) == false) {
    std::cout << "Input interface is not exist." << std::endl;
    std::cout << "Please check your interface." << std::endl;
    return -1;
  }

  WifiDeauth deauth(argv[1], argv[2], atoi(argv[3]));
  deauth.runDeauth();

  return 0;
}
