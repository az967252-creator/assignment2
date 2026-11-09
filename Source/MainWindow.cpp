#include "MainWindow.h"

#include <QBrush>
#include <QByteArray>
#include <QColor>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QStringList>
#include <QTableWidget>
#include <QVBoxLayout>

#include <exception>
#include <string>

// ---- your original Vole backend (unchanged) ----
#include "Machine.h"
#include "VoleExceptions.h"
#include "Instruction.h"

namespace {

const QColor kPcColor(255, 235, 130);      // next instruction (PC and PC+1)
const QColor kChangedColor(170, 225, 170); // changed by the last step / run
const long long kMaxInstantSteps = 1000000;

QString hexByte(int v)
{
    return QString::number(v & 0xFF, 16).rightJustified(2, QLatin1Char('0')).toUpper();
}

QString hexWord(int v)
{
    return QString::number(v & 0xFFFF, 16).rightJustified(4, QLatin1Char('0')).toUpper();
}

QString binByte(int v)
{
    const QString s = QString::number(v & 0xFF, 2).rightJustified(8, QLatin1Char('0'));
    return s.left(4) + QLatin1Char(' ') + s.mid(4);
}

QString regName(int i)
{
    return QStringLiteral("R") + QString::number(i, 16).toUpper();
}

void styleItem(QTableWidgetItem *it, const QColor &bg)
{
    if (bg.isValid()) {
        it->setBackground(QBrush(bg));
        it->setForeground(QBrush(QColor(0, 0, 0)));
    } else {
        it->setData(Qt::BackgroundRole, QVariant());
        it->setData(Qt::ForegroundRole, QVariant());
    }
}

// Human readable description of one 16-bit instruction.
QString describe(uint16_t raw)
{
    const Instruction in(raw);
    const int r = in.getR();
    const int s = in.getS();
    const int t = in.getT();
    const QString xy = hexByte(in.getXY());

    switch (in.getOpcode()) {
    case Opcode::LOAD_MEM:
        return QStringLiteral("LOAD %1 <- MEM[%2]").arg(regName(r), xy);
    case Opcode::LOAD_IMM:
        return QStringLiteral("LOAD %1 <- 0x%2").arg(regName(r), xy);
    case Opcode::STORE:
        return QStringLiteral("STORE %1 -> MEM[%2]").arg(regName(r), xy);
    case Opcode::MOVE:
        return QStringLiteral("MOVE %1 -> %2").arg(regName(s), regName(t));
    case Opcode::ADD_INT:
        return QStringLiteral("%1 <- %2 + %3 (integer)").arg(regName(r), regName(s), regName(t));
    case Opcode::ADD_FLOAT:
        return QStringLiteral("%1 <- %2 + %3 (float)").arg(regName(r), regName(s), regName(t));
    case Opcode::OR:
        return QStringLiteral("%1 <- %2 OR %3").arg(regName(r), regName(s), regName(t));
    case Opcode::AND:
        return QStringLiteral("%1 <- %2 AND %3").arg(regName(r), regName(s), regName(t));
    case Opcode::XOR:
        return QStringLiteral("%1 <- %2 XOR %3").arg(regName(r), regName(s), regName(t));
    case Opcode::ROTATE:
        return QStringLiteral("ROTATE %1 right by %2").arg(regName(r), QString::number(t));
    case Opcode::JUMP_EQ:
        return QStringLiteral("IF %1 == R0 THEN PC <- 0x%2").arg(regName(r), xy);
    case Opcode::HALT:
        return QStringLiteral("HALT");
    default:
        return QStringLiteral("INVALID instruction");
    }
}

} // namespace

// ------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), machine(new Machine)
{
    setWindowTitle(tr("Vole Machine Simulator"));
    resize(1280, 780);

    connect(&timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    buildUi();
    snapshotPrev();
    refreshAll(false);
    log(tr("Ready. Click \"Load Program...\" and choose a .vole / .txt file."));
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);

    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    // ---------------- top bar ----------------
    auto *bar = new QHBoxLayout;

    loadBtn = new QPushButton(tr("Load Program..."));
    startEdit = new QLineEdit(QStringLiteral("00"));
    startEdit->setMaxLength(2);
    startEdit->setMaximumWidth(44);
    startEdit->setAlignment(Qt::AlignCenter);
    startEdit->setToolTip(tr("Start address in hex (must be even and less than FF)"));

    stepBtn = new QPushButton(tr("Step (F10)"));
    stepBtn->setShortcut(QKeySequence(Qt::Key_F10));
    runAllBtn = new QPushButton(tr("Run All"));
    animBtn = new QPushButton(tr("Run Animated"));
    resetBtn = new QPushButton(tr("Reset"));
    addBtn = new QPushButton(tr("Add Program"));
    addBtn->setToolTip(tr("Load another program at the start address WITHOUT clearing memory, registers, PC or screen"));
    pcEdit = new QLineEdit(QStringLiteral("00"));
    pcEdit->setMaxLength(2);
    pcEdit->setMaximumWidth(44);
    pcEdit->setAlignment(Qt::AlignCenter);
    pcEdit->setToolTip(tr("New PC value in hex (even, 00-FE)"));
    setPcBtn = new QPushButton(tr("Set PC"));
    setPcBtn->setToolTip(tr("Jump the PC to this address and clear the halted flag (registers and memory are kept)"));

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(50, 1000);
    speedSlider->setValue(300);
    speedSlider->setFixedWidth(150);
    speedLabel = new QLabel(tr("300 ms"));
    speedLabel->setMinimumWidth(50);

    bar->addWidget(loadBtn);
    bar->addWidget(addBtn);
    bar->addWidget(new QLabel(tr("Start address (hex):")));
    bar->addWidget(startEdit);
    bar->addSpacing(20);
    bar->addWidget(stepBtn);
    bar->addWidget(runAllBtn);
    bar->addWidget(animBtn);
    bar->addWidget(resetBtn);
    bar->addSpacing(20);
    bar->addWidget(new QLabel(tr("PC (hex):")));
    bar->addWidget(pcEdit);
    bar->addWidget(setPcBtn);
    bar->addSpacing(20);
    bar->addWidget(new QLabel(tr("Animation delay:")));
    bar->addWidget(speedSlider);
    bar->addWidget(speedLabel);
    bar->addStretch(1);

    // ---------------- registers ----------------
    auto *regBox = new QGroupBox(tr("Registers (double-click a Hex cell to edit)"));
    auto *regLay = new QVBoxLayout(regBox);
    regTable = new QTableWidget(16, 4);
    regTable->setHorizontalHeaderLabels(QStringList() << tr("Hex") << tr("Dec") << tr("Binary") << tr("Float"));
    QStringList regNames;
    for (int i = 0; i < 16; ++i)
        regNames << regName(i);
    regTable->setVerticalHeaderLabels(regNames);
    regTable->setFont(mono);
    regTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    regTable->verticalHeader()->setDefaultSectionSize(26);
    for (int i = 0; i < 16; ++i) {
        for (int c = 0; c < 4; ++c) {
            auto *it = new QTableWidgetItem;
            it->setTextAlignment(Qt::AlignCenter);
            if (c != 0)
                it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            regTable->setItem(i, c, it);
        }
    }
    defaultTriggers = regTable->editTriggers();
    regLay->addWidget(regTable);

    // ---------------- memory ----------------
    auto *memBox = new QGroupBox(tr("Memory 16 x 16 (double-click a cell to edit)"));
    auto *memLay = new QVBoxLayout(memBox);
    memTable = new QTableWidget(16, 16);
    QStringList cols, rows;
    for (int i = 0; i < 16; ++i) {
        cols << QString::number(i, 16).toUpper();
        rows << hexByte(i * 16);
    }
    memTable->setHorizontalHeaderLabels(cols);
    memTable->setVerticalHeaderLabels(rows);
    memTable->setFont(mono);
    memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            auto *it = new QTableWidgetItem;
            it->setTextAlignment(Qt::AlignCenter);
            memTable->setItem(r, c, it);
        }
    }
    memLay->addWidget(memTable);

    auto *legend = new QLabel(
        tr("<span style=\"background:#ffeb82;color:#000\">&nbsp;yellow&nbsp;</span> = PC (next instruction) &nbsp;&nbsp;"
           "<span style=\"background:#aae1aa;color:#000\">&nbsp;green&nbsp;</span> = changed by the last step"));
    memLay->addWidget(legend);

    // ---------------- right column ----------------
    auto *cpuBox = new QGroupBox(tr("CPU"));
    auto *grid = new QGridLayout(cpuBox);
    auto addRow = [&](int row, const QString &name, QLabel *&field) {
        grid->addWidget(new QLabel(name), row, 0, Qt::AlignTop);
        field = new QLabel;
        field->setFont(mono);
        field->setWordWrap(true);
        field->setTextInteractionFlags(Qt::TextSelectableByMouse);
        grid->addWidget(field, row, 1);
    };
    addRow(0, tr("PC:"), pcLabel);
    addRow(1, tr("IR:"), irLabel);
    addRow(2, tr("Status:"), statusLabel);
    addRow(3, tr("Last executed:"), lastLabel);
    addRow(4, tr("Next:"), nextLabel);
    addRow(5, tr("Steps:"), stepsLabel);
    grid->setColumnStretch(1, 1);

    auto *screenBox = new QGroupBox(tr("Screen (writes to memory address FF)"));
    auto *screenLay = new QVBoxLayout(screenBox);
    screenView = new QPlainTextEdit;
    screenView->setReadOnly(true);
    screenView->setFont(mono);
    screenLay->addWidget(screenView);

    auto *logBox = new QGroupBox(tr("Log"));
    auto *logLay = new QVBoxLayout(logBox);
    logView = new QPlainTextEdit;
    logView->setReadOnly(true);
    logView->setMaximumBlockCount(500);
    logLay->addWidget(logView);

    auto *rightPane = new QWidget;
    auto *rightLay = new QVBoxLayout(rightPane);
    rightLay->setContentsMargins(0, 0, 0, 0);
    rightLay->addWidget(cpuBox);
    rightLay->addWidget(screenBox, 1);
    rightLay->addWidget(logBox, 1);

    // ---------------- assemble ----------------
    auto *split = new QSplitter(Qt::Horizontal);
    split->addWidget(regBox);
    split->addWidget(memBox);
    split->addWidget(rightPane);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 5);
    split->setStretchFactor(2, 3);
    split->setSizes(QList<int>() << 400 << 560 << 320);

    root->addLayout(bar);
    root->addWidget(split, 1);
    setCentralWidget(central);

    // ---------------- connections ----------------
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadProgram);
    connect(stepBtn, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddProgram);
    connect(setPcBtn, &QPushButton::clicked, this, &MainWindow::onSetPC);
    connect(runAllBtn, &QPushButton::clicked, this, [this] { toggleRun(false); });
    connect(animBtn, &QPushButton::clicked, this, [this] { toggleRun(true); });
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(regTable, &QTableWidget::itemChanged, this, &MainWindow::onRegisterEdited);
    connect(memTable, &QTableWidget::itemChanged, this, &MainWindow::onMemoryEdited);
}

// ------------------------------------------------------------------
// Buttons

void MainWindow::onLoadProgram()
{
    bool ok = false;
    const unsigned addr = startEdit->text().trimmed().toUInt(&ok, 16);
    if (!ok || addr > 0xFF) {
        showError(tr("Start address must be a hex value between 00 and FF."));
        return;
    }

    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Vole program"), lastDir,
        tr("Vole programs (*.vole *.txt);;All files (*)"));
    if (path.isEmpty())
        return;
    lastDir = QFileInfo(path).absolutePath();

    try {
        machine->reset(); // start from a clean machine
        const QByteArray local = path.toLocal8Bit();
        const int count = machine->loadProgram(
            std::string(local.constData(), static_cast<size_t>(local.size())),
            static_cast<uint8_t>(addr));
        machine->getScreen().clear(); // loading up to address FF would otherwise print to the screen
        programLoaded = true;
        stepCount = 0;
        log(tr("Loaded %1 instructions from %2 at address %3.")
                .arg(count)
                .arg(QFileInfo(path).fileName(), hexByte(static_cast<int>(addr))));
    } catch (const VoleException &e) {
        programLoaded = false;
        showError(QString::fromStdString(e.what()));
    } catch (const std::exception &e) {
        programLoaded = false;
        showError(tr("Unexpected error: ") + QString::fromStdString(e.what()));
    }

    snapshotPrev();
    refreshAll(false);
}

void MainWindow::onAddProgram()
{
    bool ok = false;
    const unsigned addr = startEdit->text().trimmed().toUInt(&ok, 16);
    if (!ok || addr > 0xFF) {
        showError(tr("Start address must be a hex value between 00 and FF."));
        return;
    }

    const QString path = QFileDialog::getOpenFileName(
        this, tr("Add a Vole program (memory is kept)"), lastDir,
        tr("Vole programs (*.vole *.txt);;All files (*)"));
    if (path.isEmpty())
        return;
    lastDir = QFileInfo(path).absolutePath();

    // Remember everything that must survive the load
    // (Machine::loadProgram resets the CPU, so registers/PC would be lost).
    const std::array<uint8_t, 16> savedRegs = machine->getCPU().getRegisters().snapshot();
    const std::array<uint8_t, 256> savedMem = machine->getMemory().snapshot();
    const uint8_t savedPC = machine->getCPU().getPC();
    const QString savedScreen = QString::fromStdString(machine->getScreen().getContents()).trimmed();
    const bool hadProgram = programLoaded;
    const bool wasHalted = machine->isHalted();

    try {
        const QByteArray local = path.toLocal8Bit();
        const int count = machine->loadProgram(
            std::string(local.constData(), static_cast<size_t>(local.size())),
            static_cast<uint8_t>(addr));

        // put back registers, PC, halted flag and screen text
        for (int i = 0; i < 16; ++i)
            machine->setRegister(i, savedRegs[i]);
        // If the old program is still in progress, keep its PC.
        // If it already halted, leave the PC on the NEW program (loadProgram
        // already set it) so that Run/Step executes the added program.
        if (hadProgram && !wasHalted)
            machine->getCPU().setPC(savedPC);
        machine->getScreen().clear();
        if (!savedScreen.isEmpty()) {
            const QStringList tokens = savedScreen.split(QLatin1Char(' '));
            for (const QString &tok : tokens)
                machine->getScreen().print(static_cast<uint8_t>(tok.toUInt(nullptr, 16)));
        }
        programLoaded = true;

        // warn if the new program overwrote cells that already held data
        const std::array<uint8_t, 256> nowMem = machine->getMemory().snapshot();
        int overwritten = 0;
        for (int a = static_cast<int>(addr); a < static_cast<int>(addr) + 2 * count && a < 256; ++a) {
            if (savedMem[a] != 0 && savedMem[a] != nowMem[a])
                ++overwritten;
        }

        log(tr("Added %1 instructions from %2 at address %3 (memory, registers, PC and screen kept).")
                .arg(count)
                .arg(QFileInfo(path).fileName(), hexByte(static_cast<int>(addr))));
        if (hadProgram && wasHalted)
            log(tr("The previous program had halted, so the PC now points to the new program at %1.")
                    .arg(hexByte(static_cast<int>(addr))));
        if (overwritten > 0)
            log(tr("[WARNING] The new program overwrote %1 cells that were not empty. Use a different start address.")
                    .arg(overwritten));

        prevRegs = savedRegs;   // so the new cells are shown in green
        prevMem = savedMem;
    } catch (const VoleException &e) {
        showError(QString::fromStdString(e.what()));
        return;
    } catch (const std::exception &e) {
        showError(tr("Unexpected error: ") + QString::fromStdString(e.what()));
        return;
    }

    refreshAll(true);
}

void MainWindow::onSetPC()
{
    if (!programLoaded) {
        showError(tr("Load a program first."));
        return;
    }
    bool ok = false;
    const unsigned newPc = pcEdit->text().trimmed().toUInt(&ok, 16);
    if (!ok || newPc > 0xFE || newPc % 2 != 0) {
        showError(tr("PC must be an even hex value between 00 and FE."));
        return;
    }

    // CPU::reset sets the PC and clears the halted flag, but also clears the
    // registers, so we save and restore them.
    const std::array<uint8_t, 16> savedRegs = machine->getCPU().getRegisters().snapshot();
    machine->getCPU().reset(static_cast<uint8_t>(newPc));
    for (int i = 0; i < 16; ++i)
        machine->setRegister(i, savedRegs[i]);

    log(tr("PC set to %1 (registers, memory and screen kept; halted flag cleared).")
            .arg(hexByte(static_cast<int>(newPc))));
    snapshotPrev();
    refreshAll(false);
}

void MainWindow::onStep()
{
    if (!canRun())
        return;
    snapshotPrev();
    const bool keepGoing = stepOnce();
    finishStepping(keepGoing);
}

void MainWindow::onReset()
{
    machine->reset();
    programLoaded = false;
    stepCount = 0;
    snapshotPrev();
    refreshAll(false);
    log(tr("Machine reset (memory, registers and screen cleared)."));
}

void MainWindow::toggleRun(bool animated)
{
    if (running) {
        setRunning(false, animated);
        log(tr("Stopped."));
        return;
    }
    if (!canRun())
        return;
    setRunning(true, animated);
}

void MainWindow::setRunning(bool on, bool animated)
{
    running = on;
    animatedMode = animated;

    if (on) {
        batchSize = animated ? 1 : 2000;
        runSteps = 0;
        timer.setInterval(animated ? speedSlider->value() : 0);
        timer.start();
    } else {
        timer.stop();
    }

    runAllBtn->setText(on && !animated ? tr("Stop") : tr("Run All"));
    animBtn->setText(on && animated ? tr("Stop") : tr("Run Animated"));
    runAllBtn->setEnabled(!on || !animated);
    animBtn->setEnabled(!on || animated);
    stepBtn->setEnabled(!on);
    loadBtn->setEnabled(!on);
    addBtn->setEnabled(!on);
    setPcBtn->setEnabled(!on);
    pcEdit->setEnabled(!on);
    resetBtn->setEnabled(!on);
    startEdit->setEnabled(!on);

    const QAbstractItemView::EditTriggers trig = on ? QAbstractItemView::NoEditTriggers : defaultTriggers;
    regTable->setEditTriggers(trig);
    memTable->setEditTriggers(trig);
}

void MainWindow::onSpeedChanged(int value)
{
    speedLabel->setText(tr("%1 ms").arg(value));
    if (running && animatedMode)
        timer.setInterval(value);
}

void MainWindow::onTimerTick()
{
    snapshotPrev();
    bool keepGoing = true;
    int done = 0;
    while (done < batchSize && keepGoing) {
        keepGoing = stepOnce();
        ++done;
    }
    runSteps += done;
    finishStepping(keepGoing);

    if (running && !animatedMode && runSteps >= kMaxInstantSteps) {
        setRunning(false, animatedMode);
        log(tr("Stopped after %1 steps without reaching HALT (probably an infinite loop).").arg(runSteps));
    }
}

// ------------------------------------------------------------------
// Execution helpers

bool MainWindow::canRun()
{
    if (!programLoaded) {
        showError(tr("No program loaded! Please load a program first."));
        return false;
    }
    if (machine->isHalted()) {
        showError(tr("The machine is halted. Load the program again or press Reset."));
        return false;
    }
    return true;
}

// Executes one instruction. Returns false when the machine halted or failed.
bool MainWindow::stepOnce()
{
    try {
        machine->step();
        ++stepCount;
        return !machine->isHalted();
    } catch (const VoleException &e) {
        stepError = QString::fromStdString(e.what());
    } catch (const std::exception &e) {
        stepError = tr("Unexpected error: ") + QString::fromStdString(e.what());
    }
    return false;
}

void MainWindow::finishStepping(bool keepGoing)
{
    refreshAll(true);

    if (!stepError.isEmpty()) {
        setRunning(false, animatedMode);
        const QString msg = stepError;
        stepError.clear();
        showError(msg);
    } else if (!keepGoing) {
        setRunning(false, animatedMode);
        log(tr("Program halted."));
    }
}

// ------------------------------------------------------------------
// Editing cells by hand

void MainWindow::onRegisterEdited(QTableWidgetItem *item)
{
    if (updating || item->column() != 0)
        return;

    bool ok = false;
    const unsigned v = item->text().trimmed().toUInt(&ok, 16);
    if (!ok || v > 0xFF) {
        showError(tr("Enter a hex value between 00 and FF."));
    } else {
        machine->setRegister(item->row(), static_cast<uint8_t>(v));
        log(tr("Register %1 set to %2.").arg(regName(item->row()), hexByte(static_cast<int>(v))));
    }
    snapshotPrev();
    refreshAll(false);
}

void MainWindow::onMemoryEdited(QTableWidgetItem *item)
{
    if (updating)
        return;

    const int addr = item->row() * 16 + item->column();
    bool ok = false;
    const unsigned v = item->text().trimmed().toUInt(&ok, 16);
    if (!ok || v > 0xFF) {
        showError(tr("Enter a hex value between 00 and FF."));
    } else {
        machine->setMemoryCell(static_cast<uint8_t>(addr), static_cast<uint8_t>(v));
        log(tr("Memory[%1] set to %2.").arg(hexByte(addr), hexByte(static_cast<int>(v))));
    }
    snapshotPrev();
    refreshAll(false);
}

// ------------------------------------------------------------------
// Display

void MainWindow::snapshotPrev()
{
    prevRegs = machine->getCPU().getRegisters().snapshot();
    prevMem = machine->getMemory().snapshot();
}

void MainWindow::refreshAll(bool highlightChanges)
{
    updating = true;

    CPU &cpu = machine->getCPU();
    const std::array<uint8_t, 16> regs = cpu.getRegisters().snapshot();
    const std::array<uint8_t, 256> mem = machine->getMemory().snapshot();
    const int pc = cpu.getPC();

    // registers
    for (int i = 0; i < 16; ++i) {
        const int v = regs[i];
        const bool changed = highlightChanges && regs[i] != prevRegs[i];
        const QString texts[4] = {
            hexByte(v),
            QString::number(v),
            binByte(v),
            QString::number(MiniFloat::decode(static_cast<uint8_t>(v)), 'g', 6)
        };
        for (int c = 0; c < 4; ++c) {
            QTableWidgetItem *it = regTable->item(i, c);
            it->setText(texts[c]);
            styleItem(it, changed ? kChangedColor : QColor());
        }
    }

    // memory
    for (int a = 0; a < 256; ++a) {
        const int v = mem[a];
        QTableWidgetItem *it = memTable->item(a / 16, a % 16);
        it->setText(hexByte(v));
        it->setToolTip(tr("Address 0x%1\nDecimal: %2\nBinary: %3")
                           .arg(hexByte(a))
                           .arg(v)
                           .arg(binByte(v)));

        QColor bg;
        if (programLoaded && (a == pc || a == ((pc + 1) & 0xFF)))
            bg = kPcColor;
        else if (highlightChanges && mem[a] != prevMem[a])
            bg = kChangedColor;
        styleItem(it, bg);
    }

    // CPU panel
    pcLabel->setText(QStringLiteral("0x") + hexByte(pc));
    irLabel->setText(QStringLiteral("0x") + hexWord(cpu.getIR()));

    QString status;
    QString color;
    if (!programLoaded) {
        status = tr("NO PROGRAM");
        color = QStringLiteral("#888888");
    } else if (cpu.isHalted()) {
        status = tr("HALTED");
        color = QStringLiteral("#c62828");
    } else {
        status = tr("RUNNING");
        color = QStringLiteral("#2e7d32");
    }
    statusLabel->setText(status);
    statusLabel->setStyleSheet(QStringLiteral("font-weight:bold; color:%1;").arg(color));

    lastLabel->setText((stepCount > 0 && cpu.getIR() != 0)
                           ? hexWord(cpu.getIR()) + QStringLiteral("  ") + describe(cpu.getIR())
                           : QStringLiteral("-"));

    if (programLoaded && !cpu.isHalted()) {
        const uint16_t next = machine->getMemory().readWord(static_cast<uint8_t>(pc));
        nextLabel->setText(hexWord(next) + QStringLiteral("  ") + describe(next));
    } else {
        nextLabel->setText(QStringLiteral("-"));
    }
    stepsLabel->setText(QString::number(stepCount));

    // screen (hex + ASCII)
    const QString rawScreen = QString::fromStdString(machine->getScreen().getContents()).trimmed();
    if (rawScreen.isEmpty()) {
        screenView->setPlainText(tr("(empty)"));
    } else {
        QStringList tokens = rawScreen.split(QLatin1Char(' '));
        QString prefix;
        const int maxShown = 1000;
        if (tokens.size() > maxShown) {
            tokens = tokens.mid(tokens.size() - maxShown);
            prefix = QStringLiteral("... ");
        }
        QString ascii;
        for (const QString &tok : tokens) {
            const unsigned v = tok.toUInt(nullptr, 16);
            ascii += (v >= 32 && v < 127) ? QChar(static_cast<int>(v)) : QChar('.');
        }
        screenView->setPlainText(tr("Hex:\n%1%2\n\nASCII:\n%1%3")
                                     .arg(prefix, tokens.join(QLatin1Char(' ')), ascii));
    }

    updating = false;
}

void MainWindow::log(const QString &msg)
{
    logView->appendPlainText(msg);
}

void MainWindow::showError(const QString &msg)
{
    log(tr("[ERROR] ") + msg);
    QMessageBox::warning(this, tr("Vole Simulator"), msg);
}