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

#ifndef WIFI_DEAUTH_H
#define WIFI_DEAUTH_H

#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <tins/tins.h>
#include <unistd.h>

class WifiDeauth
{
private:
    const Tins::NetworkInterface ifaceTrigger;
    const Tins::NetworkInterface ifaceScanner;
    const std::string strSniffer;
    const Tins::NetworkInterface::Info infoTrigger = ifaceTrigger.info();
    const Tins::NetworkInterface::Info infoScanner = ifaceScanner.info();
    const Tins::IPv4Address ipNet = Tins::IPv4Address(infoScanner.ip_addr & infoScanner.netmask);
    const Tins::IPv4Range rangeNet;
    const uint32_t channel;

    Tins::RadioTap radio = Tins::RadioTap();
    Tins::PacketSender sender;
    Tins::Dot11Deauthentication deauth = Tins::Dot11Deauthentication();

    std::map<std::string, Tins::HWAddress<6> > mapAP;
    std::vector<Tins::HWAddress<6> > vecVictims;
    std::vector<Tins::HWAddress<6> > vecFinalAP;

    bool flgPrint = true;
    uint32_t iterSniff = 500;

    bool callbackSniff(Tins::PDU &pdu);
    inline void listingAP();
    void scanVictims();
    void deauthVictims();

public:
    WifiDeauth(std::string _ifaceTrigger, std::string _ifaceScanner, uint16_t _channel);
    void runDeauth();
};

#endif // WIFI_DEAUTH_H
