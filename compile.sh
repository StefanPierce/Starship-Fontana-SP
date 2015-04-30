g++ -c src/*.cpp --std=c++11
g++ -o Starship *.o -l SDL2 -l SDL2_image
./Starship
