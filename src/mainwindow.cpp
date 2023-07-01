#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include "dashboardwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_errorLabel(nullptr)
{
    initializeUI();
    setupConnections();
}

void MainWindow::initializeUI()
{
    ui->setupUi(this);

    this->setWindowTitle("Banking Application");
    this->setStyleSheet("background-color: #96ceff;");
    this->setWindowIcon(QIcon(":/images/BankingIcon.png"));

    ui->label->setAlignment(Qt::AlignCenter);

    ui->usernameText->setPlaceholderText("Enter username");
    ui->passwordText->setPlaceholderText("Enter password");

    QPixmap bankLogo(":/images/BankingIcon.png");
    ui->label->setPixmap(bankLogo);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red; font-size: 15px");
    m_errorLabel->setGeometry(20, 525, 600, 40);
    m_errorLabel->hide();
}

void MainWindow::setupConnections()
{
    connect(ui->LoginButton, &QPushButton::released, this, &MainWindow::processLogin);
    connect(ui->RegisterButton, &QPushButton::released, this, &MainWindow::processRegistration);
}

void MainWindow::processLogin()
{
    std::string username = ui->usernameText->text().toStdString();
    std::string password = ui->passwordText->text().toStdString();

    if (username.empty() || password.empty()) {
        displayError("Please enter both username and password");
        return;
    }

    if (m_userCredentials.find(username) != m_userCredentials.end()) {
        if (m_userCredentials[username] == password) {
            qDebug() << "Login successful for user:" << QString::fromStdString(username);

            DashboardWindow *dashboard = new DashboardWindow(username, m_userBalances, m_transactions, this);
            dashboard->show();

            this->hide();
        } else {
            displayError("Incorrect password");
        }
    } else {
        displayError("User does not exist");
    }
}

void MainWindow::processRegistration()
{
    std::string username = ui->usernameText->text().toStdString();
    std::string password = ui->passwordText->text().toStdString();

    if (username.size() < 3) {
        displayError("Username must be at least 3 characters");
        return;
    }

    if (m_userCredentials.find(username) != m_userCredentials.end()) {
        displayError("Username already exists");
        return;
    }

    if (password.size() < 3) {
        displayError("Password must be at least 3 characters");
        return;
    }

    m_userCredentials[username] = password;

    m_userBalances[username] = {"100000", "20000", "0", "0", "0", "0"};

    ui->usernameText->clear();
    ui->passwordText->clear();

    displaySuccess("Registration successful! You can now log in.");

    qDebug() << "User registered successfully:" << QString::fromStdString(username);
}

void MainWindow::displayError(const std::string& errorMessage)
{
    m_errorLabel->setStyleSheet("color: red; font-size: 15px");
    m_errorLabel->setText(QString::fromStdString(errorMessage));
    m_errorLabel->show();

    qDebug() << "Error displayed:" << QString::fromStdString(errorMessage);
}

void MainWindow::displaySuccess(const std::string& message)
{
    m_errorLabel->setStyleSheet("color: green; font-size: 15px");
    m_errorLabel->setText(QString::fromStdString(message));
    m_errorLabel->show();

    qDebug() << "Success message displayed:" << QString::fromStdString(message);
}

MainWindow::~MainWindow()
{
    delete ui;

}
