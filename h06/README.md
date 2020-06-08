(Srpski):

Test primeri se nalaze u datoteci 'example'
Prvi nacin za pokretanje celog programa preko terminala:

make
bash script.sh examples/1.txt
 
Umesto 1.txt moze da bude i 2.txt, 3.txt, ?.txt ...

Drugi nacin za pokretanje programa preko terminala:

make
./program examples/1.txt
./fmm problem1.txt matrix_game
./fmm problem2.txt matrix_game

Ovaj nacin ne izbacuje lepo ispisan rad algoritma u solution.txt kao sto to prethodni nacin radi
matrix_game argument oznacava da se ne postavlja ogranicenje v >= 0 u FMM metodi za resavanje problema linearnog programiranja (inace, za sve promenljive se automatski postavlja x >=0)