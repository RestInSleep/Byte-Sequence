Protokół
PPCB służy do przesyłania ciągu bajtów pomiędzy klientem a serwerem. Jako protokołu niższej warstwy używa TCP lub UDP, a połączenia korzystające z UDP mogą obsługiwać prosty mechanizm retransmisji. Trzeba więc zaimplementować trzy wersje protokołu. Ciąg bajtów jest przesyłany w paczkach o wielkości od 1 do 64000 bajtów. Każda paczka może mieć inną długość.

Komunikacja przebiega następująco (szczegółowe opisy zawartości pakietów znajdują się poniżej):

Nawiązanie połączenia

Gdy używany jest protokół TCP, klient tworzy połączenie TCP do serwera. Jeżeli serwer obsługuje już inne połączenie, to nie odbiera nowego do czasu zakończenia obsługi poprzedniego. Jeśli klientowi nie uda się nawiązać połączenia, to kończy działanie.

Klient wysyła pakiet CONN.

Serwer odsyła pakiet CONACC, jeśli przyjmuje połączenie. Serwer korzystający z TCP zawsze odpowiada w ten sposób. Serwer korzystający z UDP może być w trakcie obsługi innego połączenia, w takiej sytuacji odsyła pakiet CONRJT. Klient po otrzymaniu CONRJT kończy działanie.

Przesyłanie danych

Dopóki nie zostaną wysłane wszystkie dane:

Klient wysyła pakiet DATA o niezerowej długości.

Serwer po otrzymaniu pakietu DATA sprawdza, czy pochodzi on z aktualnie obsługiwanego połączenia, czy pakiet jest kolejnym pakietem danych i czy jest poprawny. Jeśli nie, to serwer odsyła pakiet RJT i przestaje obsługiwać to połączenie.

Poprawny pakiet jest potwierdzany przez serwer UDP z retransmisją poprzez odesłanie pakietu ACC, a serwery TCP i UDP nie wysyłają potwierdzeń.

Klient UDP z retransmisją czeka na otrzymanie ACC przed wysłaniem kolejnego pakietu.

Zakończenie

Po odebraniu wszystkich danych serwer TCP odsyła potwierdzenie odebrania danych RCVD, zamyka połączenie i przechodzi do obsługi kolejnych. Serwery UDP i UDP z retransmisją odsyłają potwierdzenie RCVD i przechodzą do obsługi kolejnych połączeń.

Po wysłaniu wszystkich danych i odebraniu potwierdzenia (RCVD) klient TCP zamyka połączenie i kończy działanie. Klient UDP i UDP z retransmisją kończy działanie.

W prawie każdym miejscu, w którym następuje oczekiwanie na odebranie pakietu, jeśli pakiet nie będzie otrzymany w trakcie MAX_WAIT sekund, program ma zakończyć obsługę danego połączenia bądź zażądać retransmisji. Nie dotyczy to oczekiwania przez serwer na nawiązanie nowego połączenia, które trwa dowolnie długo.

Mechanizm retransmisji działa następująco: jeśli w okresie MAX_WAIT nie zostanie otrzymane potwierdzenie odebrania wysłanych danych, to należy je wysłać ponownie. Czynność tę należy powtórzyć maksymalnie MAX_RETRANSMITS razy, a w przypadku niepowodzenia zakończyć obsługę połączenia. Potwierdzeniem odebrania wysłanych danych jest odebranie kolejnego pakietu wynikającego z przebiegu działania protokołu. Przykładowo, potwierdzeniem odebrania pakietu DATA jest pakiet ACC, a potwierdzeniem odebrania ACC jest kolejny pakiet z danymi. Potwierdzeniem CONN jest CONACC. Ostatni pakiet DATA jest potwierdzany pakietem ACC, który już nie jest potwierdzany. Serwer bezpośrednio po jednokrotnym wysłaniu tego ACC wysyła pakiet RCVD, który również nie jest potwierdzany. Serwer i klient ignorują ponownie otrzymane wcześniejsze pakiety (a więc DATA z wcześniejszym numerem pakietu lub CONN w przypaku serwera, natomiast ACC z wcześniejszym numerem pakietu lub CONACC w przypadku klienta).

Pakiety przesyłane przez protokół
Przesyłane pakiety składają się z pól o określonej długości występujących bezpośrednio po sobie, bez żadnych wypełnień pomiędzy polami.

Pakiety to:

CONN: Nawiązanie połączenia (K->S)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 1,
losowy identyfikator sesji: 64 bity; można je traktować jako ciąg bajtów albo liczbę,
identyfikator protokołu: 8 bitów; wartości to:
tcp: 1
udp: 2
udp z retransmisją: 3,
długość ciągu bajtów: 64 bity; liczba w porządku sieciowym.
CONACC: Akceptacja połączenia (S->K)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 2,
identyfikator sesji: 64 bity.
CONRJT: Odrzucenie połączenia (S->K)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 3,
identyfikator sesji: 64 bity.
DATA: Pakiet z danymi (K->S)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 4,
identyfikator sesji: 64 bity,
numer pakietu: 64 bity; liczba w porządku sieciowym,
liczba bajtów danych w pakiecie: 32 bity; liczba w porządku sieciowym,
dane: długość zależna od pola 3, ciąg bajtów.
ACC: Potwierdzenie pakietu z danymi (S->K)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 5,
identyfikator sesji: 64 bity,
numer pakietu: 64 bity; liczba w porządku sieciowym.
RJT: Odrzucenie pakietu z danymi (S->K)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 6,
identyfikator sesji: 64 bity,
numer pakietu: 64 bity; liczba w porządku sieciowym.
RCVD: Potwierdzenie otrzymania całego ciągu (S->K)

identyfikator typu pakietu: 8 bitów; dla tego typu pakietów 7,
identyfikator sesji: 64 bity.

