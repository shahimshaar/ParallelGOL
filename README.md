# Game of Life implementation in C
A paralellized simulation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), implemented in C using minimum/optimal number of Pthreads.

Conway's game of life is a no-player game where there is a grid of cells, each either alive or dead, and once the game is supplied with the first state for the grid and the number of rounds to play, the rules are what create the state for each round, not the player. Here are the rules for Conway's game of life according to Wikipedia:
    1. Any live cell with two or three live neighbours survives for the next round.
    2. Any dead cell with three live neighbours becomes a live cell.
    3. All other live cells die in the next round. Similarly, all other dead cells stay dead.

I wrote this code for my CS 309 Course at Knox College, the 'skeleton' of this code was written by Professor David Bunde, before the pthreads and barriers were introduce by me.

When you run this program, it will ask for 6 input parameters in the following order:
Number of rows, number of column, likelihood for drawing initial grid (the likelihood that each cell will begin alive or dead, this simulation begins with a random grid depending on the supplied likelihood, but can be easily adapted to hard-wire particular initial states), number of rounds to simulate, number of threads to use, and whether or not you want debugging outputs.

For instance, if we want to simulate the game of life with 4 rows, 4 columns, a %100 likelihood that each initial cell is alive (full grid), 2 rounds, using 4 threads, with debugging outputs activated, we would compile the file (with gcc -pthread -o main main.c on a Linux file) and start the program with the following input:

./main 4, 4, 1.0, 2, 4, y

This input outputs the following:

The initial state:<br/>

XXXX<br/>
XXXX<br/>
XXXX<br/>
XXXX<br/>

Live count = 16<br/>

-----------------------

X..X<br/>
....<br/>
....<br/>
X..X<br/>

Live count = 4<br/>

-----------------------

....<br/>
....<br/>
....<br/>
....<br/>

Live count = 0<br/>

-----------------------

As we can see, a fully populated initial grid completely dies off within 2 rounds of the game of life.