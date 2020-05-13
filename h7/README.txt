Modifikovani Kruskalov algoritam:

Ulaz: Težinski graf G = (V, E)
Izlaz: Minimalno Razapinjujuće stablo T

Primeri:
    1.txt (iz pdf fajla)
    2.txt (sa slike primer2.png)

Kompilacija:
    make

Pokretanje:
    ./program examples/1.txt , za prvi prvimer

    ili

    ./program examples/2.txt , za drugi primer

*Rezultat se nalazi u output.txt fajlu

Kodiranje imena čvorova:
    A -> 0
    B -> 1
    C -> 2
    ...

Napomena: Struktura UnionFind se koristi za efikasno menjanje indeksa čvorova tj. u 
          složenosti O(logn), umesto niza gde bi složenost bila O(n).
