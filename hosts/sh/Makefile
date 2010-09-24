all:
	c++ -I../../include -I../../external/include -L../../external/lib/apple -Wall -g -o o3 o3_sh.cpp -ldl -lpthread -lv8
	cp o3 /bin

clean:
	rm /bin/o3
