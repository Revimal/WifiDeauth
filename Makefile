default :
	g++ -o WifiDeauth main.cpp wifi_deauth.cpp -O3 -std=c++11 -Os -flto \
		-mtune=native -march=native -ltins

clean :
	rm WifiDeauth
