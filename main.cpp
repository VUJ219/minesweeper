#include <iostream>
#include <cstdlib>
#include <ctime>

enum GameState {
    newgame,
    inprogress,
    won,
    lost,
};

struct Space {
    bool isClear;
    bool mine;
    bool flag;
    int neighbouringMines;
};

struct BoardStruct {
    Space* board;
    int x, y;
    int remainingMines;
    bool flagmode;
    GameState gameState;
};

BoardStruct setup() {
    BoardStruct game;
    do {
        std::cout << "Tell me how big should be the board and how many mines do you want in it.\n(e.g. 16 16 40)\n";
        std::cin >> game.x >> game.y >> game.remainingMines;
    } while (game.x <= 0 || game.y <= 0 || game.remainingMines < 0 || game.remainingMines >= game.x * game.y);
    game.flagmode = false;
    game.gameState = newgame;
    game.board = new Space[game.x*game.y];
    for (int i = 0; i < game.x*game.y; i++) {
        game.board[i].isClear = false;
        game.board[i].mine = false;
        game.board[i].flag = false;
        game.board[i].neighbouringMines = 0;
    }
    return game;
}

void mineplacement(BoardStruct* game, int fx, int fy) {
    int mleft = game->remainingMines;
    int randompick;
    bool beforestartingpoint = true;
    for (int i = 0; i < game->x * game->y; i++) {
        if (i != fx + fy * game->x) {
            if (beforestartingpoint)
                randompick = rand()%(game->x*game->y-1-i)+1;
            else
                randompick = rand()%(game->x*game->y-i)+1;
            if (randompick <= mleft) {
                mleft--;
                game->board[i].mine = true;
            }
        }
        else
            beforestartingpoint = false;
    }
}

void printspace(Space actualSpace, GameState gameState) {
            if (!actualSpace.isClear) {
                if ((gameState == lost || gameState == won) && actualSpace.mine == true) {
                    std::cout << "[x]";
                }
                else if (actualSpace.flag) {
                    std::cout << "[P]";
                }
                else {
                    std::cout << "[ ]";
                }
            }
            else {
                if (!actualSpace.mine) {
                    if (actualSpace.neighbouringMines == 0) {
                        std::cout << "   ";
                    }
                    else {
                        std::cout << ' ' << actualSpace.neighbouringMines << ' ';
                    }
                }
                else {
                    std::cout << ">X<";
                }
            }
}

void printboard(BoardStruct* game) {
    std::cout << "\nMines left: " <<  game->remainingMines << '\t';
    switch (game->gameState) {
        case won: std::cout << "\\(*U*)/"; break;
        case lost: std::cout << "(X_X)"; break;
        default: std::cout << "('U')";
    }
    if (game->flagmode) {
        std::cout << '\t' << "Flag mode";
    }
    std::cout << "\n\n   ";
    for (int x = 0; x < game->x; x++) {
        if (x + 1 < 10) {
            std::cout << ' ';
        }
        std::cout << x + 1;
        if (x + 1 < 100) {
            std::cout << ' ';
        }
    }
    for (int y = 0; y < game->y; y++) {
        std::cout << '\n';
        std::cout << y + 1;
        if (y + 1 < 100) {
            std::cout << ' ';
            if (y + 1 < 10) {
                std::cout << ' ';
            }
        }
        for (int x = 0; x < game->x; x++) {
            printspace(game->board[x + y * game->x], game->gameState);
        }
    }
    std::cout << "\n\n";
}

int minecounter(BoardStruct* game, int inix, int iniy) {
    int neighbouringMines = 0;
    for (int i = -1; i < 2; i++) {
        if (i+iniy >= 0 && i+iniy < game->y) {
            for (int j = -1; j < 2; j++) {
                if (j+inix>=0&&j+inix<game->x) {
                    if (game->board[j+inix+(i+iniy)*game->x].mine) {
                        neighbouringMines++;
                    }
                }
            }
        }
    }
    return neighbouringMines;
}

void clearSpace(BoardStruct* game, int inix, int iniy) {
    for (int i = -1; i < 2; i++) {
        if (i+iniy >= 0 && i+iniy < game->y) {
            for (int j = -1; j < 2; j++) {
                if (j+inix >= 0 && j+inix < game->x) {
                    if (!game->board[j+inix+(i+iniy)*game->x].isClear) {
                        game->board[inix+iniy*game->x].neighbouringMines = minecounter(game, inix, iniy);
                        if (game->board[inix+iniy*game->x].neighbouringMines == 0) {
                            game->board[j+inix+(i+iniy)*game->x].isClear = true;
                            clearSpace(game, j + inix, i + iniy);
                        }
                    }
                }
            }
        }
    }
}

int readCoordinates() {
    int input;
    std::cin >> input;
    input--;
    return input;
}

void boolSwitcher (bool& switcher) {
    if (switcher) {
        switcher = false;
    }
    else{
        switcher = true;
    }
}

void flagSwitch (BoardStruct* game, int location) {
    boolSwitcher(game->board[location].flag);
    if (game->board[location].flag) {
        game->remainingMines--;
    }
    else {
        game->remainingMines++;
    }
}

void pick(BoardStruct* game) {
    bool isOk;
    do {
        isOk = true;
        if (game->gameState == newgame) {
            std::cout << "Where do you want to start?";
        }
        else if (!game->flagmode) {
            std::cout << "Pick your next move.";
        }
        else {
            std::cout << "Pick a square for a flag.";
        }
        std::cout << " (x and y coordinates)";
        if (!game->gameState == newgame)
            std::cout << "\n(to change between normal and flag mode give 0 input)";
        std::cout << '\n';
        int input1 = readCoordinates();
        if (input1 == -1 && game->gameState == inprogress) {
            boolSwitcher(game->flagmode);
        }
        else {
            int input2 = readCoordinates();
            if (input1 >= 0 && input1 < game->x && input2 >= 0 && input2 < game->y) {
                int pick = (input1 + input2 * game->x);
                if (game->gameState == newgame) {
                    mineplacement(game, input1, input2);
                    game->gameState = inprogress;
                }
                if (game->board[pick].mine) {
                    if (!game->flagmode) {
                        game->board[pick].isClear = true;
                        game->gameState = lost;
                    }
                    else {
                        flagSwitch(game, pick);
                    }
                }
                else if (!game->board[pick].isClear) {
                    if (!game->flagmode) {
                        game->board[pick].isClear = true;
                        clearSpace(game, input1, input2);
                    }
                    else
                        flagSwitch(game, pick);
                }
                else {
                    isOk = false;
                    std::cout << "\nPick a blank square!\n\n";
                }
            }
            else {
                isOk = false;
                std::cout << "\nThe coordinates are not on the board!\n\n";
            }
        }
    } while (!isOk);
}

void end(BoardStruct* game) {
    std::cout << "\t\t";
    if (game->gameState == won)
        std::cout << "YOU WIN";
    else
        std::cout << "GAME OVER";
    printboard(game);
    delete[] game->board;
}

void checkwincon(BoardStruct* game) {
    bool iswon = true;
    if (game->remainingMines == 0) {
        for (int i = 0; i < game->x*game->y; i++)
            if (game->board[i].flag && !game->board[i].mine) {
                iswon = false;
                break;
            }
    }
    else {
        for (int i = 0; i < game->x*game->y; i++)
            if (!game->board[i].mine && !game->board[i].isClear) {
                iswon=false;
                break;
        }
    }
    if (iswon)
        game->gameState = won;
}

bool playagain() {
    char yesno;
    do {
        std::cout << "Do you want to play again? (y/n)\n";
        std::cin >> yesno;
    } while (yesno != 'y' && yesno != 'n');
    std::cout << '\n';
    if (yesno == 'y')
        return true;
    else
        return false;
}

int main(){
    srand(time(NULL));
    do {
        BoardStruct game = setup();
        while (game.gameState == inprogress || game.gameState == newgame) {
            printboard(&game);
            pick(&game);
            checkwincon(&game);
        }
        end(&game);
    } while (playagain());
}
