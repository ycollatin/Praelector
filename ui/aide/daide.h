#ifndef DAIDE_H
#define DAIDE_H

#include <QWebPage>

class DAide : public QWebPage
{
    Q_OBJECT

public:
    DAide(QWidget *parent = 0);
};

#endif
