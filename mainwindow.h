#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>

        QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    std::string werdeAufgerufen();
    void printGrid(int grid[9][9]);
    void generateSudoku(int grid[9][9]);
    void removeNumbers(int grid[9][9], int numToRemove);
    void setMaxPlayer();
    void checkGridComplete(int grid[9][9]);
    void printScoreboard();



private:
    Ui::MainWindow *ui;
    QIntValidator* validator;

private slots:
    void generate();
    void submit();
    void player();
    void reset();
};
#endif // MAINWINDOW_H
