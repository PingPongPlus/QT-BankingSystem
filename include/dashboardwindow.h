#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <map>
#include <string>
#include <vector>
#include <QCloseEvent>

namespace Ui {
class DashboardWindow;
}

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(const std::string& username,
                             std::map<std::string, std::vector<std::string>>& sharedMapBalance,
                             std::map<std::string, std::vector<std::string>>& sharedTransactions,
                             QWidget *parent = nullptr);

    ~DashboardWindow();

private slots:
    void showInvestView();
    void showInfoView();
    void showHomeView();
    void showTransferView();
    void signOut();
    void buyBitcoin();
    void sellBitcoin();
    void processTransfer();
    void updateBitcoinPrice();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initializeUserData();
    void setupUserInterface();
    void connectSignalsAndSlots();
    void loadImages();
    void updateBalanceDisplay();
    void updateTransactionHistory();
    void updateInvestmentValue(const QString& currentPriceStr);

    Ui::DashboardWindow *ui;
    // Balance slots: cash, savings, investment value, principal, entry price, latest price.
    std::map<std::string, std::vector<std::string>>& m_balanceMap;
    std::map<std::string, std::vector<std::string>>& m_transactions;
    std::string m_username;
    bool m_isInvested;

    const QString m_redValueStyle = R"(
        QLabel {
            color: red;
            font-family: "San Francisco", "Helvetica Neue", "Segoe UI", "Arial", sans-serif;
            font-size: 45px;
            font-weight: 600;
            padding: 10px 22px;
            border: none;
            border-radius: 14px;
            min-height: 40px;
            min-width: 120px;
            letter-spacing: 0.4px;
            background: none;
        })";

    const QString m_greenValueStyle = R"(
        QLabel {
            color: #00d119;
            font-family: "San Francisco", "Helvetica Neue", "Segoe UI", "Arial", sans-serif;
            font-size: 45px;
            font-weight: 600;
            padding: 10px 22px;
            border: none;
            border-radius: 14px;
            min-height: 40px;
            min-width: 120px;
            letter-spacing: 0.4px;
            background: none;
        })";
};

#endif // DASHBOARDWINDOW_H
