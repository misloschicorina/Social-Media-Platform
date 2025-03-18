# Social Media Platform - Graph & Data Structures

**Misloschi Alexandra Corina**  
**314 CA, 2023-2024**

    TASK 1:

    Pentru implementarea primului task am folosit un graf pentru a putea
manipula cat mai usor conexiunile dintre nodurile reprezentate de useri.
Nodul unui user din graf are structura `user_node_t` si contine id-ul user-ului, 
cat si username-ul (campurile principale care-l definesc pe utilizatorul 
retelei). Graful are structura `graph_t` si este un graf reprezentat prin liste de
adiacenta, 
fiecare lista `friends_list_t` continand prietenii user-ului.
    Pentru implementarea comenzilor posibile legate de gestionarea prietenilor,
mi-am definit atat functii auxiliare, cat si functii principale care rezolva 
cerinta. Ele sunt urmatoarele:

- create_graph - creeaza graful de prietenii (cu un numar dat de utilizatori);
- add_friend_to_list - adauga un user dat intr-o lista de prieteni, plasandu-l 
intotdeauna la inceputul listei;
- add_edge - creeaza o legatura de prietenie intre doi utilizatori, adaugandu-i 
reciproc in listele lor de prieteni;
- remove_friend_from_list - elimina un prieten specificat din lista de prieteni 
a unui utilizator;
- remove_edge - sterge legatura de prietenie dintre doi utilizatori, 
eliminandu-i reciproc din listele lor de prieteni;
- calc_distance - calculeaza distanta minima dintre doi utilizatori in graf, 
utilizand o parcurgere de tip BFS;
- is_friend - verifica daca doi utilizatori sunt prieteni;
- sort_array -  sorteaza un vector de intregi in ordine crescatoare folosind 
algoritmul de sortare prin insertie;
- suggestions - ofera sugestii de prieteni pentru un utilizator, bazandu-se pe 
prietenii prietenilor sai care nu sunt deja prieteni directi;
- common_friends - gaseste si afiseaza prietenii comuni intre doi utilizatori, 
comparand listele lor de prieteni;
- print_nr_friends - afiseaza numarul de prieteni al unui utilizator specificat;
- nr_friends - returneaza numarul de prieteni al unui utilizator, pe baza 
dimensiunii listei sale de prieteni;
- most_popular - determina si afiseaza cel mai popular prieten dintre prietenii 
unui utilizator;
- free_graph - elibereaza complet memoria alocata grafului si a elementelor 
asociate.

    TASK 2:

    Avem o lista dublu inlantuita de arbori. Fiecare nod al listei `ll_node_t`
este radacina unui arbore generic. Nodul reprezinta postarea in sine si 
proprietatile ei :
    *numarul de like-uri
    *lista de utilizatori care au dat like
    *daca e postare sau repostare
    *id-ul postarii originale (0 daca ea insusi e postare)
    *id-ul postarii
    *id-ul userului care a facut postarea
    *repostarile care vor deriva din ea ("copiii") (`g_node_t **repost`)

    In lista dublu inlantuita se adauga si repostarile, avand aceeasi structura
 ca si postarile.

    Functiile necesare implementarii functionalitatii acestui task sunt 
urmatoarele:

- create_node - creeaza un nod pentru un arbore generic;
- create_list - creeaza o lista dublu inlantuita goala;
- create_tree -  creeaza un arbore generic avand ca radacina o postare;
- add_node - adauga un nod nou in lista dublu inlantuita;
- create_post - creeaza o postare noua si o adauga in lista de postari;
- add_child_to_node - adauga un copil la un nod din arborele generic;
- find_repost - cauta o repostare specifica intr-un arbore generic;
- free_node_recursive - elibereaza recursiv memoria pentru un nod si copiii sai;
- create_repost - creeaza o repostare si o adauga in arborele postarii 
originale;
- print_reposts - afiseaza recursiv toate repostarile unui nod;
- get_reposts -  afiseaza toate repostarile unei postari specifice;
- update_likes - actualizeaza numarul de like-uri pentru o postare sau 
repostare;
- handle_likes - gestioneaza like-urile pentru o postare sau repostare;
- get_likes - afiseaza numarul de like-uri pentru o postare sau repostare;
- find_max_likes - gaseste postarea sau repostarea cu cele mai multe like-uri 
intr-un arbore;
- ratio - verifica daca repostarea cu cele mai multe like-uri are mai multe 
like-uri decat postarea originala;
- free_list - elibereaza memoria pentru intreaga lista dublu inlantuita si 
arborii asociati;
- delete_tree - sterge recursiv un arbore generic;
- remove_child - elimina un copil dintr-un nod din arborele generic;
- delete_post_from_list - sterge o postare din lista dublu inlantuita;
- delete_repost - sterge o repostare specifica din arbore;
- delete_post - sterge o postare din lista dublu inlantuita si arborele asociat;
- lca - gaseste cel mai apropiat stramos comun (LCA) dintre doua noduri dintr-un 
arbore;
- find_common_repost - gaseste prima repostare comuna pentru doua repostari date 
dintr-un arbore;

    TASK 3:

    Implementarea ultimului task implica gestionarea si afisarea feed-ului 
utilizatorilor intr-o retea sociala, identificarea prietenilor care au repostat 
anumite postari, vizualizarea de profil si determinarea grupului de prieteni 
cel mai apropiat.
    Tocmai pentru a gestiona mai simplu acest task, am adaugat la task-ul 
anterior , atat postarile pe lista dublu inlantuita, cat si repostarile (pentru 
a rezuma functiile de afisare la o singura parcurgere liniara a listei).

    Functiile implementate in cadrul task-ului sunt urmatatoarele:

- show_feed : Afiseaza ultimele nr_posts postari originale ale prietenilor si 
ale utilizatorului curent, ignorand repostarile. Parcurge lista dublu inlantuita 
de postari de la sfarsit catre inceput si verifica daca utilizatorul curent sau 
prietenii sai au realizat postarea.

- traverse_repost_tree : Parcurge recursiv arborele de repostari pentru a gasi 
nodul corespunzator unui repost_id specific. Compara id-ul repostarii cu fiecare 
nod si returneaza nodul daca gaseste o potrivire.

- get_post_title_from_repost : Gaseste titlul postarii originale pentru o 
repostare data. Parcurge lista de postari si traverseaza arborele de repostari 
pentru a gasi repostarea respectiva, apoi returneaza titlul postarii originale.

- view_profile : Afiseaza toate postarile si repostarile facute de un utilizator 
dat. Parcurge lista de postari si verifica daca utilizatorul curent a realizat 
fiecare postare sau repostare, afisand titlul postarii sau repostarii.

- show_friends_that_reposted : Afiseaza prietenii unui utilizator care au 
repostat o postare anume. Parcurge lista de postari si verifica daca fiecare 
repostare a fost realizata de un prieten al utilizatorului curent, afisand 
numele prietenului.

- is_clique :  Verifica daca un grup de utilizatori reprezinta o clica 
(fiecare utilizator este prieten cu toti ceilalti utilizatori din grup). 
Parcurge toate perechile de utilizatori din grup si verifica daca fiecare 
pereche este legata de prietenie.

- find_largest_clique :  Gaseste cea mai mare clica din graful de prietenii 
pentru un utilizator specificat. Creeaza toate submultimile posibile de prieteni 
ai utilizatorului si verifica daca fiecare submultime este o clica, pastrand cea 
mai mare clica gasita.

- show_group : Afiseaza cel mai apropiat grup de prieteni (cea mai mare clica) 
pentru un utilizator dat. Gaseste cea mai mare clica care include 
utilizatorul curent si afiseaza numele utilizatorilor din aceasta clica.
