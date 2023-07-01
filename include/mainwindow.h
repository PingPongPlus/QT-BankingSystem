#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <map>
#include <string>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void processLogin();
    void processRegistration();

private:
    void initializeUI();
    void setupConnections();
    void displayError(const std::string& errorMessage);
    void displaySuccess(const std::string& message);

    Ui::MainWindow *ui;
    QLabel *m_errorLabel;

    std::map<std::string, std::string> m_userCredentials;
    std::map<std::string, std::vector<std::string>> m_userBalances;
    std::map<std::string, std::vector<std::string>> m_transactions;
};
#endif // MAINWINDOW_H
