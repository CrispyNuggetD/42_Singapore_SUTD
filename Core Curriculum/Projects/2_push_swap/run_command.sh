make re && make generator && clear;
ARG=$(./bin/generator 500); ./push_swap $ARG | ./tests/checker_linux $ARG