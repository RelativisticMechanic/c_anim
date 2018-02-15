g++ -c f_sdl.cpp -o f_sdl.o
g++ -c c_anim.cpp -o c_anim.o
g++ -c l_timer.cpp -o l_timer.o

ar rvs canim.a f_sdl.o c_anim.o l_timer.o
del *.o
pause
