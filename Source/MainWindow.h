#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractItemView>
#include <QMainWindow>
#include <QString>
#include <QTimer>

#include <array>
#include <cstdint>
#include <memory>

class Machine;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSlider;
class QTableWidget;
class QTableWidgetItem;

// Qt front-end for the Vole machine. It only talks to the existing
// Machine class, so none of your original simulator code is changed.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLoadProgram();
    void onAddProgram();
    void onSetPC();
    void onStep();
    void onReset();
    void onTimerTick();
    void onSpeedChanged(int value);
    void onRegisterEdited(QTableWidgetItem *item);
    void onMemoryEdited(QTableWidgetItem *item);

private:
    void buildUi();
    void toggleRun(bool animated);
    void setRunning(bool on, bool animated);
    bool canRun();
    bool stepOnce();
    void finishStepping(bool keepGoing);
    void snapshotPrev();
    void refreshAll(bool highlightChanges);
    void log(const QString &msg);
    void showError(const QString &msg);

    std::unique_ptr<Machine> machine;
    QTimer timer;

    bool programLoaded = false;
    bool running = false;
    bool animatedMode = false;
    bool updating = false;          // true while we fill the tables ourselves
    int batchSize = 1;
    int stepCount = 0;
    long long runSteps = 0;
    QString stepError;
    QString lastDir;
    QAbstractItemView::EditTriggers defaultTriggers;

    std::array<uint8_t, 16> prevRegs{};
    std::array<uint8_t, 256> prevMem{};

    // widgets
    QPushButton *loadBtn = nullptr;
    QPushButton *addBtn = nullptr;
    QPushButton *setPcBtn = nullptr;
    QLineEdit *pcEdit = nullptr;
    QPushButton *stepBtn = nullptr;
    QPushButton *runAllBtn = nullptr;
    QPushButton *animBtn = nullptr;
    QPushButton *resetBtn = nullptr;
    QLineEdit *startEdit = nullptr;
    QSlider *speedSlider = nullptr;
    QLabel *speedLabel = nullptr;
    QTableWidget *regTable = nullptr;
    QTableWidget *memTable = nullptr;
    QLabel *pcLabel = nullptr;
    QLabel *irLabel = nullptr;
    QLabel *statusLabel = nullptr;
    QLabel *lastLabel = nullptr;
    QLabel *nextLabel = nullptr;
    QLabel *stepsLabel = nullptr;
    QPlainTextEdit *screenView = nullptr;
    QPlainTextEdit *logView = nullptr;
};

#endif // MAINWINDOW_H
