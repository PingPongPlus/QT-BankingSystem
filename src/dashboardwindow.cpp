#include "dashboardwindow.h"
#include "ui_dashboardwindow.h"
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QCloseEvent>
#include <QApplication>

DashboardWindow::DashboardWindow(const std::string& username,
                                 std::map<std::string, std::vector<std::string>>& sharedMapBalance,
                                 std::map<std::string, std::vector<std::string>>& sharedTransactions,
                                 QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DashboardWindow)
    , m_balanceMap(sharedMapBalance)
    , m_transactions(sharedTransactions)
    , m_username(username)
    , m_isInvested(false)
{
    ui->setupUi(this);
    ui->lineEdit->setPlaceholderText("Enter amount to invest");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DashboardWindow::updateBitcoinPrice);
    timer->start(30000); // Limit requests to the live price API.

    initializeUserData();
    setupUserInterface();
    connectSignalsAndSlots();

    updateBitcoinPrice();
}

void DashboardWindow::initializeUserData()
{
    qDebug() << "Initializing user data for:" << QString::fromStdString(m_username);

    if (m_balanceMap.find(m_username) == m_balanceMap.end()) {
        m_balanceMap[m_username] = {"100000", "20000", "0", "0", "0"};
        qDebug() << "No existing balance found. Setting default balances.";
    }

    if (m_transactions.find(m_username) == m_transactions.end()) {
        m_transactions[m_username] = {};
        qDebug() << "No existing transactions found. Initializing empty history.";
    }

    updateBalanceDisplay();
    updateTransactionHistory();
}

void DashboardWindow::setupUserInterface()
{
    this->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    this->setStyleSheet("background-color: #f5f5f7;");
    this->setWindowIcon(QIcon(":/images/BankingIcon.png"));

    ui->LabelName->setAlignment(Qt::AlignCenter);
    ui->LabelName->setText(QString::fromStdString(m_username));
    ui->usernameCard->setText(QString::fromStdString(m_username));
    ui->usernameCard_3->setText(QString::fromStdString(m_username));

    ui->textEdit->setReadOnly(true);

    loadImages();
}

void DashboardWindow::connectSignalsAndSlots()
{
    connect(ui->HomeButton, &QPushButton::released, this, &DashboardWindow::showHomeView);
    connect(ui->InfoButton, &QPushButton::released, this, &DashboardWindow::showInfoView);
    connect(ui->SignoutButton, &QPushButton::released, this, &DashboardWindow::signOut);
    connect(ui->TransferButton, &QPushButton::released, this, &DashboardWindow::showTransferView);
    connect(ui->SendButton, &QPushButton::released, this, &DashboardWindow::processTransfer);
    connect(ui->InvestButton, &QPushButton::released, this, &DashboardWindow::showInvestView);
    connect(ui->Invest, &QPushButton::released, this, &DashboardWindow::buyBitcoin);
    connect(ui->Sell, &QPushButton::released, this, &DashboardWindow::sellBitcoin);
}

void DashboardWindow::loadImages()
{
    QPixmap bankIcon(":/images/BankingIcon.png");
    bankIcon = bankIcon.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Icon->setPixmap(bankIcon);

    QPixmap bankSolo(":/images/BankSolo.png");
    bankSolo = bankSolo.scaled(179, 179, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Icon_2->setPixmap(bankSolo);

    ui->Icon_4->setPixmap(QPixmap(":/images/Bank2.png").scaled(ui->Icon_4->size(), Qt::KeepAspectRatio));
    ui->label_15->setPixmap(QPixmap(":/images/evm.png").scaled(ui->label_15->size(), Qt::KeepAspectRatio));
    ui->label_4->setPixmap(QPixmap(":/images/evm.png").scaled(ui->label_4->size(), Qt::KeepAspectRatio));

    QPixmap bitcoinIcon(":/images/Bitcoin.png");
    bitcoinIcon = bitcoinIcon.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->BitcoinIcon->setPixmap(bitcoinIcon);
}

void DashboardWindow::updateBalanceDisplay()
{
    const auto& balances = m_balanceMap[m_username];
    ui->BalanceCard1->setText(QString::fromStdString(balances[0] + "$"));
    ui->BalanceCard2_2->setText(QString::fromStdString(balances[1] + "$"));
}

void DashboardWindow::updateTransactionHistory()
{
    ui->listWidget->clear();
    for (const std::string &transaction : m_transactions[m_username]) {
        ui->listWidget->addItem(QString::fromStdString(transaction));
    }
}

void DashboardWindow::showInvestView()
{
    qDebug() << "Switching to Investment view";
    ui->stackedWidget->setCurrentIndex(3);
}

void DashboardWindow::showInfoView()
{
    ui->stackedWidget->setCurrentIndex(0);
    qDebug() << "Switched to Information view.";
}

void DashboardWindow::showHomeView()
{
    ui->stackedWidget->setCurrentIndex(2);
    qDebug() << "Switched to Home view.";
}

void DashboardWindow::showTransferView()
{
    ui->stackedWidget->setCurrentIndex(1);
    qDebug() << "Switched to Transfer view.";
}

void DashboardWindow::signOut()
{
    this->hide();
    if (parentWidget()) {
        parentWidget()->show();
    }
    qDebug() << "User signed out.";
}

void DashboardWindow::buyBitcoin()
{
    if (m_isInvested) {
        qDebug() << "Already invested. Ignoring buy request.";
        return;
    }

    bool conversionOk = false;
    int investedCash = ui->lineEdit->text().toInt(&conversionOk);

    if (!conversionOk || investedCash <= 0) {
        qDebug() << "Invalid Amount: Please enter a valid positive amount.";
        return;
    }

    auto& userBalance = m_balanceMap[m_username];
    int currentBalance = std::stoi(userBalance[0]);

    if (investedCash > currentBalance) {
        qDebug() << "Insufficient Funds: You don't have enough balance to invest.";
        return;
    }
    int newBalance = currentBalance - investedCash;
    userBalance[0] = std::to_string(newBalance);

    userBalance[2] = std::to_string(investedCash);
    userBalance[3] = std::to_string(investedCash);

    QString priceText = ui->BitcoinPrice->text().split(":").last().remove("$").trimmed();
    userBalance[4] = priceText.toStdString();

    m_isInvested = true;

    updateBalanceDisplay();

    QString transaction = QString("Bought Bitcoin: %1 $ at %2 $/BTC").arg(investedCash).arg(priceText);
    m_transactions[m_username].push_back(transaction.toStdString());
    ui->listWidget->addItem(transaction);

    qDebug() << "Investment made at BTC price:" << QString::fromStdString(userBalance[4]);
    qDebug() << "New balance is:" << QString::fromStdString(userBalance[0]);

    ui->label->setText(QString("%1$").arg(investedCash));
    ui->lineEdit->clear();
}

void DashboardWindow::sellBitcoin()
{
    if (!m_isInvested) {
        qDebug() << "No Investment: You do not have any active investment to sell.";
        return;
    }
    auto& userBalance = m_balanceMap[m_username];

    int investmentValue = std::stoi(userBalance[2]);
    int currentBalance = std::stoi(userBalance[0]);

    int newBalance = currentBalance + investmentValue;
    userBalance[0] = std::to_string(newBalance);

    userBalance[2] = "0";
    userBalance[3] = "0";
    userBalance[4] = "0";

    updateBalanceDisplay();

    QString transaction = QString("Sold Bitcoin: %1 $ returned to balance").arg(investmentValue);
    m_transactions[m_username].push_back(transaction.toStdString());
    ui->listWidget->addItem(transaction);

    m_isInvested = false;
    ui->label->setText("");

    qDebug() << "Investment sold. New balance: " << QString::fromStdString(userBalance[0]);
}

void DashboardWindow::processTransfer()
{
    int sourceCardIndex = ui->combobox->currentIndex();
    QString receiverUsername = ui->WhoLineEdit->text();
    QString message = ui->message->text();

    if (receiverUsername.isEmpty()) {
        qDebug() << "Missing Recipient: Please enter a recipient username.";
        return;
    }

    bool conversionOk = false;
    int amount = ui->amount->text().toInt(&conversionOk);

    if (!conversionOk || amount <= 0) {
        qDebug() << "Invalid Amount: Please enter a valid positive amount.";
        return;
    }
    std::string receiverStr = receiverUsername.toStdString();

    if (m_username == receiverStr) {
        qDebug() << "Invalid Transfer: You cannot transfer money to yourself.";
        return;
    }

    if (m_balanceMap.find(receiverStr) == m_balanceMap.end()) {
        qDebug() << "Unknown Recipient: The recipient does not have an account.";
        return;
    }

    if (std::stoi(m_balanceMap[m_username][sourceCardIndex]) < amount) {
        qDebug() << "Insufficient Funds: You don't have enough balance in the selected account.";
        return;
    }

    m_balanceMap[m_username][sourceCardIndex] = std::to_string(
        std::stoi(m_balanceMap[m_username][sourceCardIndex]) - amount
        );

    m_balanceMap[receiverStr][sourceCardIndex] = std::to_string(
        std::stoi(m_balanceMap[receiverStr][sourceCardIndex]) + amount
        );

    bool isAnonymous = ui->checkBox->isChecked();
    QString senderName = isAnonymous ? "Anonymous" : QString::fromStdString(m_username);
    QString receiverName = isAnonymous ? "Anonymous" : receiverUsername;

    QString senderTransaction = QString("-%1 $ to %2 from Card-%3 | Msg: %4")
                                    .arg(amount)
                                    .arg(receiverName)
                                    .arg(sourceCardIndex + 1)
                                    .arg(message);

    QString receiverTransaction = QString("+%1 $ from %2 from Card-%3 | Msg: %4")
                                      .arg(amount)
                                      .arg(senderName)
                                      .arg(sourceCardIndex + 1)
                                      .arg(message);

    m_transactions[m_username].push_back(senderTransaction.toStdString());
    m_transactions[receiverStr].push_back(receiverTransaction.toStdString());

    ui->listWidget->addItem(senderTransaction);
    updateBalanceDisplay();

    ui->WhoLineEdit->clear();
    ui->amount->clear();
    ui->message->clear();

    qDebug() << "Transfer Complete: Your transfer has been completed successfully.";
    qDebug() << "Transfer successful!";
}

void DashboardWindow::updateBitcoinPrice()
{
    QString executableDir = QCoreApplication::applicationDirPath();

    // The executable lives in build/<configuration>, two levels below the project root.
    QDir projectRootDir(executableDir);
    projectRootDir.cdUp();
    projectRootDir.cdUp();

    QString scriptPath = projectRootDir.absoluteFilePath("scripts\\getBTCPrice.py");

    if (!QFile::exists(scriptPath)) {
        qDebug() << "Bitcoin price script not found at:" << scriptPath;
        return;
    }

    QProcess process;
    QString command = QString("python \"%1\"").arg(scriptPath);

    process.start(command);
    if (!process.waitForFinished(5000)) {
        qDebug() << "Bitcoin price script timed out";
        return;
    }

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty()) {
        qDebug() << "Error from Bitcoin price script:" << error;
        return;
    }

    QString trimmedPrice = output.trimmed();

    ui->BitcoinPrice->setText("1 BTC: " + trimmedPrice + "$");

    auto& userBalance = m_balanceMap[m_username];
    if (userBalance.size() < 6) {
        userBalance.resize(6, "0");
    }
    userBalance[5] = trimmedPrice.toStdString();

    updateInvestmentValue(trimmedPrice);
}

void DashboardWindow::updateInvestmentValue(const QString& currentPriceStr)
{
    if (!m_isInvested) {
        return;
    }

    auto& userBalance = m_balanceMap[m_username];

    try {
        int bitcoinPriceNow = currentPriceStr.toInt();
        int bitcoinPriceAtInvestment = std::stoi(userBalance[4]);
        int originalInvestment = std::stoi(userBalance[3]);

        float percentageChange = static_cast<float>(bitcoinPriceNow - bitcoinPriceAtInvestment) / bitcoinPriceAtInvestment;
        float newValue = originalInvestment * (1.0f + percentageChange);

        userBalance[2] = std::to_string(static_cast<int>(newValue));

        QString valueStr = QString::fromStdString(userBalance[2]) +
                           " $ (" + QString::number(percentageChange * 100, 'f', 2) + " %)";
        ui->label->setText(valueStr);

        if (bitcoinPriceNow < bitcoinPriceAtInvestment) {
            ui->label->setStyleSheet(m_redValueStyle);
        } else {
            ui->label->setStyleSheet(m_greenValueStyle);
        }

        qDebug() << "Investment updated: Original:" << originalInvestment
                 << "New Value:" << newValue
                 << "Change:" << percentageChange * 100 << "%";

    } catch (const std::exception &e) {
        qDebug() << "Error in investment calculation:" << e.what();
    }
}

void DashboardWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "DashboardWindow close event detected.";

    event->accept();

    // Closing the dashboard exits the app; signing out only hides the window.
    QApplication::quit();
}

DashboardWindow::~DashboardWindow()
{
    delete ui;
    qDebug() << "DashboardWindow destroyed.";
}
