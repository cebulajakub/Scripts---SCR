#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <climits>

using namespace std;

class Taczka {
public:
    int id;
    string mineral;
    int waga;
    int ilosc;
    int czas_przyjscie;
    int czas_rozladunek;

    Taczka(int _id, string _mineral, int _waga, int _ilosc, int _czas_przyjscie, int _czas_rozladunek) :
            id(_id), mineral(_mineral), waga(_waga), ilosc(_ilosc), czas_przyjscie(_czas_przyjscie),
            czas_rozladunek(_czas_rozladunek) {}
};

class Robot {
public:
    int id;
    vector<Taczka> taczki;


    Robot(int _id) : id(_id) {}
};


bool jakisrobotzajety(const vector<Robot>& roboty) {
    for (const auto& robot : roboty) {
        if (!robot.taczki.empty()) {
            return true;
        }
    }
    return false;
}


void fcfs(vector<Taczka>& taczki_p, int ilosc_robotow) {
    int moment = 0;

    vector<Robot> roboty;
    for (int i = 0; i < ilosc_robotow; i++) {
        roboty.push_back(Robot(i + 1));
    }

    while (!taczki_p.empty() || jakisrobotzajety(roboty)) {
        cout << "Moment: " << moment << endl;

        while (!taczki_p.empty() && taczki_p.front().czas_przyjscie <= moment) {
            if (taczki_p.front().czas_przyjscie == moment) {
                cout << "        wheelbarrow arrived <" << taczki_p.front().id << " "
                     << taczki_p.front().mineral << " " << taczki_p.front().waga << " "
                     << taczki_p.front().ilosc << " [" << taczki_p.front().czas_rozladunek << "]>"
                     << endl;
            }
            bool przypisanie_taczki = false;
            for (auto& robot : roboty) {
                if (robot.taczki.empty() || robot.taczki.front().czas_rozladunek < moment) {
                if(robot.taczki.empty()){
                    robot.taczki.push_back(taczki_p.front());
                    taczki_p.erase(taczki_p.begin());
                    przypisanie_taczki = true;
                    break;
                    }
                }
            }
            if (!przypisanie_taczki) {
                break;
            }
        }

        for (auto& robot : roboty) {
            cout << "                [";
            if (!robot.taczki.empty() && robot.taczki.front().czas_rozladunek > 0) {
                Taczka& taczka = robot.taczki.front();
                cout << taczka.mineral << " " << taczka.czas_rozladunek;
                taczka.czas_rozladunek--;
                if (taczka.czas_rozladunek == 0) {
                    robot.taczki.erase(robot.taczki.begin());
                }
            }
            cout << "]";
        }
        cout << endl;

        moment++;
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Użycie: " << argv[0] << " [plik_danych]"<< "[liczba_robotow]" << endl;
        return 1;
    }

    string plik_wej_nazwa = argv[1];
    int liczba_robotow = atoi(argv[2]); 
    ifstream plik_wej;

    if (plik_wej_nazwa != "-") {
        plik_wej.open(plik_wej_nazwa);
        if (!plik_wej.is_open()) {
            cerr << "Nie można otworzyć pliku z danymi wejściowymi!" << endl;
            return 1;
        }
    }

    vector<Taczka> taczki;
    int id, waga, ilosc, czas_przybycia;
    string mineral, linia;

    if (plik_wej_nazwa == "-") {
        while (true) {
            cout<< "Podaj dane wejściowe w formie: [minutę_nadejścia taczki] [numer_minerału] [nazwa_minerału] [waga_minerału] [ilość_minerału]"<< endl;
            cout << "Wpisz 'koniec', aby zakończyć wprowadzanie danych." << endl;

            getline(cin, linia);
            if (linia == "koniec") {
                break;
            }

            stringstream wej(linia);
            wej >> czas_przybycia;
            while (wej >> id >> mineral >> waga >> ilosc) {
                Taczka przychodzaca_taczka(id, mineral, waga, ilosc, czas_przybycia, waga * ilosc);
                taczki.push_back(przychodzaca_taczka);

            }
        }
    } else {
        while (getline(plik_wej, linia)) {
            istringstream wej(linia);
            wej >> czas_przybycia;
            while (wej >> id >> mineral >> waga >> ilosc) {
                Taczka przychodzaca_taczka(id, mineral, waga, ilosc, czas_przybycia, waga * ilosc);
                taczki.push_back(przychodzaca_taczka);
            }
        }
    }

    fcfs( taczki,liczba_robotow);

    return 0;
}
