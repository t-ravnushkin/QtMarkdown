#include "editorwindow.h"
#include "ui_editorwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QSettings>

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);
    ui->preview->setContextMenuPolicy(Qt::NoContextMenu);

    settings = new QSettings("trollbisto", "markdown");

    setWindowTitle(QString("Markdown Editor"));

    ui->input->setPlainText(QString("# New file"));

    connect(ui->input, &QPlainTextEdit::textChanged, this, [this]() {ui->preview->setMarkdown(ui->input->toPlainText());});

    connect(ui->actionNew, &QAction::triggered, this, &EditorWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &EditorWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &EditorWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &EditorWindow::onFileSaveAs);

    connect(ui->input->document(), &QTextDocument::modificationChanged, ui->actionSave, &QAction::setEnabled);
}

EditorWindow::~EditorWindow()
{
    delete ui;
    delete settings;
}


void EditorWindow::openFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, windowTitle(), QString("couldn't open file %1: %2").arg(QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
    settings->setValue("filepath", path);
    m_filePath = path;
    ui->input->setPlainText(f.readAll());
}

bool EditorWindow::isModified() const {
    return ui->input->document()->isModified();
}

void EditorWindow::onFileNew() {
    EditorWindow* nw = new EditorWindow();
    nw->ui->input->setPlainText("# New file");
    nw->show();
}

void EditorWindow::onFileOpen() {
    QFileDialog dialog(this, QString("Open File"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Accepted) {
        EditorWindow* nw = new EditorWindow();
        nw->openFile(dialog.selectedFiles().constFirst());
        nw->show();
    }
}

void EditorWindow::onFileSaveAs() {
    QFileDialog dialog(this, QString("Save File"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() != QDialog::Accepted)
        return;
    m_filePath = dialog.selectedFiles().constFirst();
    settings->setValue("filepath", m_filePath);
    onFileSave();
}

void EditorWindow::onFileSave() {
    if (m_filePath.isEmpty()) {
        onFileSaveAs();
        return;
    }
    QFile f(m_filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, windowTitle(), QString("Couldn't save file to %1: %2").arg(QDir::toNativeSeparators(m_filePath), f.errorString()));
        return;
    }
    QTextStream str(&f);
    str << ui->input->toPlainText();
    ui->input->document()->setModified(false);
}

void EditorWindow::closeEvent(QCloseEvent *e) {
    if (isModified()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, windowTitle(), QString("You have unsaved changes. Do you want to quit anyway?"));
        if (button != QMessageBox::Yes)
            e->ignore();
    }
}
