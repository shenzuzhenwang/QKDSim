
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>

class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(QWidget *parent = nullptr) : QDialog(parent, Qt::Window | Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TranslucentBackground); // 可选：背景透明

        QLabel *loadingLabel = new QLabel(this);
        QMovie *loadingMovie = new QMovie("../GUI/loading.gif");
        loadingLabel->setMovie(loadingMovie);
        loadingMovie->start();

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(loadingLabel);
        setLayout(layout);

        adjustSize();  // 调整对话框大小以适应内容
    }
};


#endif // PROGRESSBAR_H
