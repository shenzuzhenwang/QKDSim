
#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent)
    : QWidget{parent}
{
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(progressBar);
    setLayout(layout);
}

void ProgressBar::updateProgress(int value)
{
    progressBar->setValue(value);
}
