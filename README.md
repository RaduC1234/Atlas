# Project Atlas

## Building

Requirements: vcpkg, cmake \
\
Dependencies:
1. [GLM](https://github.com/g-truc/glm)
2. [SFML](https://www.sfml-dev.org/index.php)
3. [ENTT](https://github.com/skypjack/entt)
4. [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)

### Tasks

- Pentru task-uri săptămânale [aici](https://github.com/users/RaduC1234/projects/6/views/1).
- Exista 2 branchuri: unul main pe care sa **NU faceti push direct si nici pull request**, si unul de test

## Battle city

Jocul [Battle city](https://docs.google.com/document/d/1ZUAht8qgf-_eWXlgzBdUDsHmSIjltmKt4VtcDRZftMs/edit?tab=t.0#heading=h.7qep3k3doi7) se desfășoară într-un spațiu bidimensional în care mai mulți jucători se luptă pentru a distruge tancurile inamice și să câștige teritoriul tabelei de joc. Pornind de ideea acestui joc să se implementeze o aplicație care respectă următoarele reguli:
- Într-un spațiu minat sunt plasate cel mult 4 personaje (câte unul pentru fiecare jucător) în unul din colțurile tabelei de joc. Fiecare personaj se va putea deplasa cu câte o unitate în una din cele 4 direcții (sus, jos, stânga sau dreapta) dacă este liberă, cu scopul de a-i împușca pe inamici.
- Spațiul de joc este reprezentat sub formă dreptunghiulară (n\*m metri, valori alese random la începutul fiecărui joc), iar distanța dintre oricare două căsuțe vecine este de 1 metru. Un personaj poate ocupa o singură căsuță la un moment dat.
- Tabela de joc conține 3 tipuri:
  - spații libere pe care se pot deplasa personajele
  - ziduri pe care jucătorii le pot sparge
  - ziduri care nu pot fi sparte
- Configurația inițială a hărții va fi generată random la începutul fiecărui joc.
- Fiecare personaj deține o armă care poate fi folosită pentru a lansa un glonț într-una dintre direcții. Dacă glonțul ajunge la unul dintre adversari, acesta “moare” și este resetat la poziția inițială. Dacă acest lucru se repetă de 3 ori, un personaj este eliminat din joc. Câștigător va fi jucătorul care reușește să-și elimine toți adversarii și să rămână singur pe tabela de joc.
- La finalul fiecărui joc, jucătorii au asociat un număr de puncte direct proporțional cu numărul de inamici împușcați. Pentru fiecare inamic împușcat aceștia vor primi 100 de puncte. Jucătorul care va câștiga un joc primește extra 200 puncte.
- Când un cont nou este creat, jucătorul va primi un pistol care poate lansa un glonț la cel puțin 4 secunde, acesta deplasându-se cu o viteză de 0,25 metri pe secundă.
- Dacă 2 gloanțe se întâlnesc acestea se vor anula și nu vor avea efecte asupra jucătorilor. Dacă un zid este spart, acesta poate declanșa o bombă care va omorî tot ce se află pe o rază de 10 m pătrați (jucători și ziduri). Fiecare hartă poate conține cel mult 3 astfel de bombe ascunse.
- În portofoliul fiecărui jucător se află:
  - Un număr de puncte obținute în urma meciurilor jucate. Aceste puncte pot fi folosite pentru a crește puterea armei. Cu un număr de 500 de puncte se poate înjumătăți timpul de așteptare dintre gloanțe de cel mult 4 ori.
  - Un scor obținut pe baza câștigurilor avute. La finalul fiecărui meci cel care a câștigat meciul primește 2 puncte de scor, iar cel de pe locul 2 primește un punct. Când un utilizator strânge 10 puncte de scor poate (o singură dată când este atins acest prag) duplica viteza de deplasare a gloanțelor.
  - Poate exista și un joc amical în care 2 jucători pot forma o echipă pentru a lupta împotriva altor echipe formate din același număr de persoane. Fiecare jucător va fi eliminat din joc după contactul cu un singur glonț, iar acest tip de joc nu va fi recompensat cu puncte.

### Cerințe de bază
- Rețelistică: implementarea aplicației respectând arhitectura client-server (aplicația trebuie să asigure posibilitatea creării a minim 2 instanțe de client + 1 aplicație server care vor comunica prin rețea).
- Pagină de Login/Register: la pornire, unui utilizator i se oferă posibilitatea de a se loga în contul său sau își poate crea un cont. Logarea/înregistrarea presupune introducerea numelui de utilizator. Atenție: numele de utilizator trebuie să fie unic (utilizarea email-ului sau a parolei este opțională)!
- Pagina jocului: va fi o aplicație consolă, care respectă regulile jocului enunțate anterior. Pentru identificarea personajelor se pot folosi litere.
- Bază de date: Pentru a vă organiza datele, puteți să le stocați într-o bază de date. Se va folosi biblioteca de SQLite SQLite ORM (NU ALTA). Se poate instala într-un proiect de Visual Studio folosind Microsoft vcpkg (vedeți aici și la curs).

### Componente avansate
- Multigaming: Suportarea mai multor jocuri în același timp. Jucătorii se pot conecta oricând și aceștia vor fi distribuiți în meciuri în jocuri câte 4, în funcție de scorul acestora. Un utilizator poate aștepta cel mult 30 de secunde. După trecerea acestui timp jocul va porni dacă există cel puțin 2 doritori indiferent de scorul acestora.
- GUI: Să se implementeze o interfață grafică a aplicației, respectând cerințele de mai sus. Aceasta trebuie să fie personalizată și să NU se asemene cu versiunea originală a jocului.

### Idei pentru elementul custom
- Definirea unui nivel de dificultate: ușor, mediu sau greu. Valoarea acestuia poate fi aleasă la începutul unui joc sau poate crește progresiv pe măsură ce jocul avansează (rămâne la alegerea voastră). Pe baza acestui nivel se va modifica dinamica și numărul “surprizelor” din interiorul jocului.
- Tratarea excepțiilor și implementarea unit testelor (se vor utiliza framework-uri specifice, de ex. Visual Studio Enterprise Edition), code coverage 50% SAU minim 40 de teste diverse (funcționalitate, backend, frontend, etc).
- Scene hotswapping via DLL (o sa ne ajute cand facem jocul propriu zis)
- Isometric Camera perspective
- **Daca mai aveti idei puteti sa le adaugati aici.**

## Concept art pentru joc 
![Image](https://github.com/user-attachments/assets/886782b6-95c8-4015-a0e8-372e41e6fdde)
