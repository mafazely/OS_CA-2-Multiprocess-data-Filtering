target:
	g++ -c my_main.cpp
	g++ -c my_loadBalancer.cpp
	g++ -c Maindistributor.cpp
	g++ -c province.cpp
	g++ -c city.cpp
	g++ -c store.cpp
	g++ -c Tools.cpp
	g++ -o StoreCalculator.out‬‬ my_main.o my_loadBalancer.o Tools.o
	g++ -o distributor Maindistributor.o Tools.o
	g++ -o province province.o Tools.o
	g++ -o city city.o Tools.o
	g++ -o store store.o Tools.o

clean:
	rm *.o
	rm ‫‪StoreCalculator.out‬‬ distributor province city store
