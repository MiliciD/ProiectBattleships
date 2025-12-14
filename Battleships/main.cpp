#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cctype>

using namespace std;

const int BOARD_SIZE = 10;

enum class CellState {
    Empty, // fara nava, nelovit
    Ship, // cu nava, nelovit
    Hit, // cu nava, lovit
    Miss // fara nava, lovit
};

class Board { // Tabbla de joc de 10 x 10
private:
    CellState grid[BOARD_SIZE][BOARD_SIZE]; // starea fiecarei celule

public:
    Board() { // constructor: initializeaza tabla goala
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                grid[i][j] = CellState::Empty;
            }
        }
    }

    bool isInside(int row, int col) const { // verifica daca coordonatele sunt in interiorul tablei
        return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
    }

    bool canPlaceShip(int row, int col, int length, bool horizontal) const { // verifica daca o nava poate fi plasata
        if (horizontal) {
            if (col + length > BOARD_SIZE) return false; // iese din tabla
            for (int j = col; j < col + length; ++j) {
                if (grid[row][j] != CellState::Empty) return false;
            }
        } else {
            if (row + length > BOARD_SIZE) return false;
            for (int i = row; i < row + length; ++i) {
                if (grid[i][col] != CellState::Empty) return false; // suprapunere
            }
        }
        return true;
    }

    bool placeShip(int row, int col, int length, bool horizontal) { // plaseaza o nava pe tabla
        if (!canPlaceShip(row, col, length, horizontal)) {
            return false;
        }

        if (horizontal) {
            for (int j = col; j < col + length; ++j) {
                grid[row][j] = CellState::Ship;
            }
        } else {
            for (int i = row; i < row + length; ++i) {
                grid[i][col] = CellState::Ship;
            }
        }
        return true;
    }

    CellState hit(int row, int col) {
        // proceseaza o lovitura la coordonatele date
        if (!isInside(row, col)) {
            cout << "Coordonate in afara tablei!\n";
            return CellState::Empty;
        }

        if (grid[row][col] == CellState::Ship) { // nava lovita
            grid[row][col] = CellState::Hit;     // marcheaza ca lovita
            cout << "Lovitura reusita! (" << row << ", " << col << ")\n";
            return CellState::Hit;
        } else if (grid[row][col] == CellState::Empty) {
            grid[row][col] = CellState::Miss;
            cout << "Ai lovit apa. (" << row << ", " << col << ")\n"; // marcheaza ca miss
            return CellState::Miss;
        } else {
            cout << "Ai mai tras deja in aceasta pozitie. ("    // deja lovit anterior
                 << row << ", " << col << ")\n";
            return grid[row][col];
        }
    }

    CellState getCell(int row, int col) const { 
        if (!isInside(row, col)) {
            return CellState::Empty; // coordonate invalide
        }
        return grid[row][col];
    }

    bool allShipsSunk() const {                 // verifica daca toate navele au fost scufundate
        for (int i = 0; i < BOARD_SIZE; ++i)    // pentru a alege un castigator
            for (int j = 0; j < BOARD_SIZE; ++j)
                if (grid[i][j] == CellState::Ship)
                    return false;
        return true;
    }

    // DEBUG: scufunda toate navele de pe aceasta tabla
    void sinkAllShips() {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (grid[i][j] == CellState::Ship) {
                    grid[i][j] = CellState::Hit;
                }
            }
        }
    }

    void print(bool revealShips = true) const { // afiseaza tabla in consola
        cout << "   ";
        for (int j = 0; j < BOARD_SIZE; ++j)
            cout << j << " ";
        cout << "\n";

        for (int i = 0; i < BOARD_SIZE; ++i) {
            cout << i << "  ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                char c;
                switch (grid[i][j]) {
                    case CellState::Empty: c = '.'; break;
                    case CellState::Ship: c = revealShips ? 'O' : '.'; break;
                    case CellState::Hit:  c = 'X'; break;
                    case CellState::Miss: c = '*'; break;
                }
                cout << c << " ";
            }
            cout << "\n";
        }
    }
};

struct ShipDef {
    string name;
    int length;
    bool horizontal; // true = orizontal, false = vertical
};

vector<ShipDef> getShipTypes() {
    // definirea a 5 tipuri de nave standardizate
    vector<ShipDef> ships;
    ships.push_back({"Nava lungime 3 (orizontala)", 3, true});
    ships.push_back({"Nava lungime 4 (orizontala)", 4, true});
    ships.push_back({"Nava lungime 2 (orizontala)", 2, true});
    ships.push_back({"Nava inaltime 2 (verticala)", 2, false});
    ships.push_back({"Nava inaltime 3 (verticala)", 3, false});
    return ships;
}

// helper pentru conversie string -> int (fara stoi)
bool parseCoord(const string& s, int& value) {
    if (s.empty()) return false;
    for (char ch : s) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    int v = 0;
    for (char ch : s) {
        v = v * 10 + (ch - '0');
    }
    value = v;
    return true;
}

void placeShipsManually(Board& board, const vector<ShipDef>& shipTypes) {
    // Plasare manuala pentru jucator
    cout << "\n--- Plasare nave pentru JUCATOR (MANUAL) ---\n";

    for (const auto& ship : shipTypes) {
        bool placed = false;

        while (!placed) {
            cout << "\nPlasati: " << ship.name 
                 << " (lungime = " << ship.length 
                 << ", orientare = " 
                 << (ship.horizontal ? "orizontala" : "verticala")
                 << ")\n";

            board.print(true);

            string rowInput, colInput;
            int r, c;

            cout << "Introduceti LINIA de START (0-9): ";
            if (!(cin >> rowInput)) {
                cout << "Input intrerupt. Iesire din plasarea navelor...\n";
                return;
            }

            if (!parseCoord(rowInput, r) || r < 0 || r >= BOARD_SIZE) {
                cout << "Coordonate gresite, te rog alege coordonate intre 0 si 9.\n";
                continue;
            }

            cout << "Introduceti COLOANA de START (0-9): ";
            if (!(cin >> colInput)) {
                cout << "Input intrerupt. Iesire din plasarea navelor...\n";
                return;
            }

            if (!parseCoord(colInput, c) || c < 0 || c >= BOARD_SIZE) {
                cout << "Coordonate gresite, te rog alege coordonate intre 0 si 9.\n";
                continue;
            }

            if (!board.canPlaceShip(r, c, ship.length, ship.horizontal)) {
                cout << "Nava nu poate fi plasata aici (iese din tabla sau se suprapune).\n";
                continue;
            }

            board.placeShip(r, c, ship.length, ship.horizontal);
            cout << "Nava plasata cu succes!\n";
            placed = true;
        }
    }

    cout << "\nTabla finala a JUCATORULUI:\n";
    board.print(true);
}

// Random pentru oricine, cu mesaj custom (mai rapid pentru a incepe jocul)
void placeShipsRandomGeneric(Board& board, const vector<ShipDef>& shipTypes, const string& ownerName, bool showBoardAfter) {
    cout << "\n--- " << ownerName << " isi plaseaza navele ALEATOR ---\n";

    for (const auto& ship : shipTypes) {
        bool placed = false;
        while (!placed) {
            int r = rand() % BOARD_SIZE;
            int c = rand() % BOARD_SIZE;

            if (board.canPlaceShip(r, c, ship.length, ship.horizontal)) {
                board.placeShip(r, c, ship.length, ship.horizontal);
                placed = true;
            }
        }
    }

    cout << ownerName << " a plasat toate navele.\n";

    if (showBoardAfter) {
        cout << "\nTabla " << ownerName << " (nave vizibile pentru debug):\n";
        board.print(true);
    }
}

// pentru computer
void placeShipsRandomlyForComputer(Board& board, const vector<ShipDef>& shipTypes) {
    placeShipsRandomGeneric(board, shipTypes, "Computerul", false);
}

void computerTurn(Board& playerBoard) {
    cout << "\n--- Randul COMPUTERULUI ---\n";
    int r, c;

    while (true) {
        r = rand() % BOARD_SIZE;
        c = rand() % BOARD_SIZE;

        CellState state = playerBoard.getCell(r, c);
        if (state == CellState::Empty || state == CellState::Ship)
            break;
    }

    cout << "Computerul loveste la: (" << r << ", " << c << ")\n";
    playerBoard.hit(r, c);
}

// Rezultatul turei jucatorului
enum class PlayerTurnResult {
    Normal,     // a tras normal
    WinCheat,   // a folosit codul "win"
    LoseCheat,  // a folosit codul "lose"
    InputExit   // EOF / stream inchis
};

// Mutarea jucatorului + coduri DEBUG "win" / "lose"
PlayerTurnResult playerTurn(Board& computerBoard) {
    cout << "\n--- Randul JUCATORULUI ---\n";

    while (true) {
        string rowInput;
        cout << "Introduceti LINIA de atac (0-9) sau cod debug (win / lose): ";
        if (!(cin >> rowInput)) {
            cout << "Input intrerupt.\n";
            return PlayerTurnResult::InputExit;
        }

        // coduri debug aici la "linie"
        if (rowInput == "win") {
            cout << "[DEBUG] Cod 'win' activat: victorie instant pentru jucator!\n";
            return PlayerTurnResult::WinCheat;
        }
        if (rowInput == "lose") {
            cout << "[DEBUG] Cod 'lose' activat: infrangere instant pentru jucator!\n";
            return PlayerTurnResult::LoseCheat;
        }

        int r;
        if (!parseCoord(rowInput, r) || r < 0 || r >= BOARD_SIZE) {
            cout << "Coordonate gresite, te rog alege coordonate intre 0 si 9.\n";
            continue;
        }

        string colInput;
        cout << "Introduceti COLOANA de atac (0-9): ";
        if (!(cin >> colInput)) {
            cout << "Input intrerupt.\n";
            return PlayerTurnResult::InputExit;
        }

        int c;
        if (!parseCoord(colInput, c) || c < 0 || c >= BOARD_SIZE) {
            cout << "Coordonate gresite, te rog alege coordonate intre 0 si 9.\n";
            continue;
        }

        if (!computerBoard.isInside(r, c)) {
            cout << "Coordonate gresite, te rog alege coordonate intre 0 si 9.\n";
            continue;
        }

        CellState state = computerBoard.getCell(r, c);
        if (state == CellState::Hit || state == CellState::Miss) {
            cout << "Ai mai lovit aici. Alege alta pozitie.\n";
            continue;
        }

        computerBoard.hit(r, c);
        return PlayerTurnResult::Normal;
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // initializare generator de numere aleatoare

    Board playerBoard;
    Board computerBoard;
    vector<ShipDef> ships = getShipTypes();

    // === ALEGERE MOD PLASARE NAVE PENTRU JUCATOR ===
    while (true) {
        cout << "Doriti plasarea navelor manual (m) sau aleatoare (a)? ";

        string line;
        if (!std::getline(cin >> std::ws, line)) {
            cout << "Input invalid. Iesire din joc.\n";
            return 0;
        }

        if (line.empty()) {
            cout << "Optiune invalida. Alegeti 'm' sau 'a'.\n";
            continue;
        }

        char choice = static_cast<char>(tolower(static_cast<unsigned char>(line[0])));

        if (choice == 'm') {
            placeShipsManually(playerBoard, ships);
            break;
        } else if (choice == 'a') {
            placeShipsRandomGeneric(playerBoard, ships, "Jucatorul", true);
            break;
        } else {
            cout << "Optiune invalida. Alegeti 'm' sau 'a'.\n";
        }
    }

    // Plasare nave pentru computer
    placeShipsRandomlyForComputer(computerBoard, ships);

    cout << "\n=== INCEPE JOCUL ===\n";

    while (true) {
        cout << "\nTabla JUCATORULUI:\n";
        playerBoard.print(true);

        cout << "\nTabla COMPUTERULUI:\n";
        computerBoard.print(false);

        // Mutarea jucatorului (poate activa si codurile debug)
        PlayerTurnResult result = playerTurn(computerBoard);

        if (result == PlayerTurnResult::InputExit) {
            cout << "\nInput intrerupt. Iesire din joc.\n";
            break;
        }
        if (result == PlayerTurnResult::WinCheat) {
            computerBoard.sinkAllShips();
        }
        if (result == PlayerTurnResult::LoseCheat) {
            playerBoard.sinkAllShips();
        }

        // Verificam dupa randul jucatorului
        if (computerBoard.allShipsSunk()) {
            cout << "\nAI CASTIGAT! Toate navele computerului au fost scufundate!\n";
            break;
        }
        if (playerBoard.allShipsSunk()) {
            cout << "\nAI PIERDUT! Toate navele tale au fost scufundate!\n";
            break;
        }

        // Mutarea computerului
        computerTurn(playerBoard);

        if (playerBoard.allShipsSunk()) {
            cout << "\nAI PIERDUT! Toate navele tale au fost scufundate!\n";
            break;
        }
    }

    cout << "\nJoc terminat.\n";
    return 0;
}

