default :
	g++ -o WifiDeauth main.cpp wifi_deauth.cpp -O3 -std=c++11 -ltins

clean :
	rm *.o WifiDeauth
