rm solution.txt
./program $1 >> solution.txt
echo "Solution for the first problem (min -v) using Fourier–Motzkin elimination:" >> solution.txt
./fmm problem1.txt matrix_game >> solution.txt
echo "" >> solution.txt
echo "Solution for the second problem (min v) using Fourier–Motzkin elimination:" >> solution.txt
./fmm problem2.txt matrix_game >> solution.txt
rm problem1.txt
rm problem2.txt