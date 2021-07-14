#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow();

  protected:
    void closeEvent(QCloseEvent *event) override;

  private slots:
    void newGame();
    void about();

  private:
    void createActions();
    bool confirmExit();
};