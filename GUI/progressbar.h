
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H


#include <QWidget>
#include <QProgressBar>
#include <QThread>
#include <QVBoxLayout>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);

public slots:
    void updateProgress(int value);

signals:

private:
    QProgressBar *progressBar;
};

#endif // PROGRESSBAR_H
