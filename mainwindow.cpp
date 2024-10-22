#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <iostream>
#include <random>
#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font("Courier New");  //Monospace-Schriftart
    ui->gridLabel->setFont(font);

    //Überprüfung ob Eingaben korrekt sind
    validator = new QIntValidator(0, 9, this);
    ui->rowEdit->setValidator(validator);
    ui->colEdit->setValidator(validator);
    ui->numEdit->setValidator(validator);
    ui->playerInEdit->setValidator(validator);
}

MainWindow::~MainWindow()
{
    delete ui;
}


const int SIZE = 9;
int grid[SIZE][SIZE];
int userRow;
int userCol;
int userNum;
int maxPlayer;
int numToRemove = 20;
int currentPlayer = 1;
int points[9] = {0};


// Methode zum anzeigen des Gitters
void MainWindow::printGrid(int grid[SIZE][SIZE]) {
    QString gridText;

    // Zeichne obere Wand
    gridText += "+";
    for (int col = 0; col < SIZE; col++) {
        gridText += "---";
        gridText += "+";
    }
    gridText += "<br>";

    // Zeichne Zeilen
    for (int row = 0; row < SIZE; row++) {
        // Zeichne Zahlen und Kästen
        for (int col = 0; col < SIZE; col++) {
            gridText += "|";

            int value = grid[row][col];
            if (value == 0) {
                gridText += " . ";
            } else {
                gridText += " " + QString::number(value) + " ";
            }
        }
        gridText += "|<br>";

        // Zeichne Trennlinie
        gridText += "+";
        for (int col = 0; col < SIZE; col++) {
            gridText += "---";
            gridText += "+";
        }
        gridText += "<br>";
    }

    // Setze den Text des Labels auf den erstellten Gitter-Text
    ui->gridLabel->setText(gridText);
}

// Funktion zur Überprüfung, ob eine Zahl in einer bestimmten Zeile gültig ist
bool isValidInRow(int grid[SIZE][SIZE], int row, int num) {
    for (int col = 0; col < SIZE; ++col) {
        if (grid[row][col] == num) {
            return false;
        }
    }
    return true;
}

// Funktion zur Überprüfung, ob eine Zahl in einer bestimmten Spalte gültig ist
bool isValidInCol(int grid[SIZE][SIZE], int col, int num) {
    for (int row = 0; row < SIZE; ++row) {
        if (grid[row][col] == num) {
            return false;
        }
    }
    return true;
}

// Funktion zur Überprüfung, ob eine Zahl in einem bestimmten 3x3-Unterquadrat gültig ist
bool isValidInBox(int grid[SIZE][SIZE], int startRow, int startCol, int num) {

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (grid[startRow + row][startCol + col] == num) {
                return false;
            }
        }
    }
    return true;
}

// Funktion zur Überprüfung, ob eine Zahl an einer bestimmten Position gültig ist
bool isValidNumber(int grid[SIZE][SIZE], int row, int col, int num) {
    return isValidInRow(grid, row, num) && isValidInCol(grid, col, num) && isValidInBox(grid, row - row % 3, col - col % 3, num);
}

//Funktion zur erhöhung des aktuellen Spielers
void increasePlayer(int& currentPlayer, int maxPlayer) {
    currentPlayer++;
    if (currentPlayer > maxPlayer) {
        currentPlayer = 1;
    }
}

//Methode zur Ausgabe des Scoreboards auf der GUI
void MainWindow::printScoreboard() {
    QString scoreboardText;

    scoreboardText += "<b>Scoreboard:</b><br>";

    for (int player = 1; player <= maxPlayer; player++) {
        scoreboardText += "Player " + QString::number(player) + ": " + QString::number(points[player - 1]) + "<br>";
    }

    // Setze den Text des Labels auf das Scoreboard
    ui->gridLabel->setText(scoreboardText);
}

//Methode zur Überprüfung, ob das Sudoku vom Spieler vollstädnig gefüllt wurde
void MainWindow::checkGridComplete(int grid[SIZE][SIZE]) {
    bool isComplete = true;

    // Durchsuche das Grid nach Nullen
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (grid[row][col] == 0) {
                isComplete = false;
                break;
            }
        }
        if (!isComplete) {
            break;
        }
    }

    // Setze den Text des Labels basierend auf dem Ergebnis
    if (isComplete) {
        printScoreboard();
    }
}

//Methode zur Adjustierung der maximalen Spieleranzahl
void MainWindow::setMaxPlayer() {
    int newMaxPlayer = ui->playerInEdit->text().toInt();
    if (newMaxPlayer > 0) {
        maxPlayer = newMaxPlayer;
    }
}

//Funktion zur erhöhung des Punktestand des aktuellen Spielers
void addPoints(int currentPlayer) {
    // Überprüfen, ob currentPlayer gültig ist
    if (currentPlayer < 1 || currentPlayer > 9) {
        // Ungültiger Spieler, Methode beenden
        return;
    }

    // Spielerpunkte erhöhen
    points[currentPlayer - 1]++;

}

// Funktion zur Lösung des Sudoku-Gitters mithilfe des Backtracking-Algorithmus
bool solveSudoku(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (grid[row][col] == 0) {
                for (int num = 1; num <= SIZE; ++num) {
                    if (isValidNumber(grid, row, col, num)) {
                        grid[row][col] = num;
                        if (solveSudoku(grid)) {
                            return true;
                        }
                        grid[row][col] = 0; // Zurücksetzen, wenn die aktuelle Belegung nicht zur Lösung führt
                    }
                }
                return false; // Wenn keine gültige Zahl gefunden wurde, ist das Sudoku unlösbar
            }
        }
    }
    return true; // Sudoku wurde vollständig gefüllt
}

// Methode zur zufälligen generierung eines Sudoku-Gitters
void MainWindow::generateSudoku(int grid[SIZE][SIZE])
{
    // Leeres Gitter initialisieren
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            grid[row][col] = 0;
        }
    }

    // generiert den Startwert (Seed) für mt19937
    std::random_device rd;
    //generiert die Random zahlen mit Seed
    std::mt19937 g(rd());

    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    //array mit hilfe von g random anordnen
    std::shuffle(std::begin(numbers), std::end(numbers), g);

    // Sudoku-Hauptdiagonale mit den zufälligen Zahlen füllen
    for (int row = 0; row < SIZE; row++) {
        //int numIndex = (row % 3) * 3 + (row / 3);
        int num = numbers[row];
        grid[row][row] = num;
    }

    // Sudoku-Gitter lösen
    solveSudoku(grid);
}

// Methode zur zufälligen Entfernung von Zahlen aus dem valid Sudoku-Gitter
void MainWindow::removeNumbers(int grid[SIZE][SIZE], int numToRemove) {
    //zufällige Zahlen generator
    std::random_device rd;
    std::mt19937 g(rd());

    int count = 0;

    while (count < numToRemove) {

        //generiert zufällige Zahlen für row und col im Bereich von 0-8
        // also von der 1. bis zur 9.m zeile/spalte. g ist
        int row = std::uniform_int_distribution<>(0, SIZE - 1)(g);
        int col = std::uniform_int_distribution<>(0, SIZE - 1)(g);

        if (grid[row][col] != 0) {
            grid[row][col] = 0;
            count++;
        }
    }
}

//Button Continue:
void MainWindow::player()
{
    setMaxPlayer();
    ui->playerLabel->setText("Current Player: " + QString::number(currentPlayer));

    ui->playerInFrame->setEnabled(false);
    ui->frame->setEnabled(true);
    ui->numToRemEdit->setFocus();
}

//Button Start Game:
void MainWindow::generate()
{

    QString userNumToRem = ui->numToRemEdit->text();
    int numToRemove = userNumToRem.toInt();

    if( numToRemove < 81 && numToRemove > 0) {

        generateSudoku(grid);

        removeNumbers(grid, numToRemove);

        printGrid(grid);

        ui->inputFrame->setEnabled(true);
        ui->rowEdit->setFocus();
        ui->frame->setEnabled(false);
        ui->gridFrame->setEnabled(true);

    }else{
        ui->numToRemEdit->clear();
    }
}

//Button Submit:
void MainWindow::submit()
{
    QString userRowIn = ui->rowEdit->text();
    int userRow = userRowIn.toInt()-1;

    QString userColIn = ui->colEdit->text();
    int userCol = userColIn.toInt()-1;

    QString userNumIn = ui->numEdit->text();
    int userNum = userNumIn.toInt();

    if(isValidNumber(grid,userRow,userCol,userNum))
    {
        grid[userRow][userCol] = userNum;
        printGrid(grid);
        addPoints(currentPlayer);
        checkGridComplete(grid);
    }else {
        increasePlayer(currentPlayer, maxPlayer);
        ui->playerLabel->setText("Current Player: " + QString::number(currentPlayer));
    }

    ui->rowEdit->clear();
    ui->colEdit->clear();
    ui->numEdit->clear();
    ui->rowEdit->setFocus();
}

//Button RESET:
void MainWindow::reset()
{
    ui->playerInEdit->clear();
    ui->numToRemEdit->clear();
    ui->playerInFrame->setEnabled(true);
    ui->frame->setEnabled(false);
    ui->inputFrame->setEnabled(false);
    ui->gridFrame->setEnabled(false);
    ui->playerInEdit->setFocus();
    ui->gridLabel->setText("Restarting...");
    ui->playerLabel->setText("Current Player: ");
    currentPlayer=1;
    for (int i = 0; i < 9; i++) {
        points[i] = 0;
    }
}
