/*       ch.h     */

#ifndef CH_H
#define CH_H

#include <QMap>
#include <QRegExp>
#include <QString>
#include <QStringList>

namespace Ch
{
    QStringList const abrev  = QStringList()
        <<"Agr"<<"Ap"<<"A"<<"K"<<"D"<<"F"<<"C"
        <<"Cn"<<"L"<<"Mam"<<"M\""<<"M"<<"N"<<"Oct"
        <<"Opet"<<"Post"<<"Pro"<<"P"<<"Q"<<"Sert"
        <<"Ser"<<"Sex"<<"S"<<"St"<<"Ti"<<"T"<<"V"
        <<"Vol"<<"Vop"<<"Pl";
    QString           accentue(QString l);
    void              allonge(QString *f);
    QStringList const anteposes = QStringList()
        << "la"
        << "le"
        << "les"
        << "leur"
        << "lui"
        << "me"
        << "nous"
        << "se"
        << "te"
        << "vous";
    QString           atone(const QString a, bool bdc = false);
    QStringList const cas = QStringList()
        << "nominatif"
        << "vocatif"
        << "accusatif"
        << "génitif"
        << "datif"
        << "ablatif"
        << "locatif";
    QStringList const personnes = QStringList()
        <<"1ère"
        <<"2ème"
        <<"3ème";
    QStringList const modes = QStringList()
        << "indicatif"
        << "subjonctif"
        << "impératif"
        << "infinitif"
        << "participe"
        << "gérondif"
        << "adjectif verbal";
    void              deQuant(QString *c);
    QString const     consonnes = "bcdfgjklmnpqrstvxz";
    QStringList const genres = QStringList()
        << "masculin" << "féminin" << "neutre";
    QString           deramise(QString r);
    void              elide(QString *mp);
    QStringList const nombres = QStringList()
        << "singulier" << "pluriel";
    const QRegExp     reAlphas("(\\w+)");
    const QRegExp     reEspace("\\s+");
    const QRegExp     reLettres("\\w");
    const QRegExp     rePonct("([\\.?!;:]|\\n\\n)");
    QChar const       separSyll = 0x02CC;
    QString           transforme(QString k);
    QString           versPC(QString k);
    QString           versPedeCerto(QString k);
    QString const     voyelles = "āăēĕīĭōŏūŭȳўĀĂĒĔĪĬŌŎŪŬȲЎ";
    // couleurs
    QString const     noir = "000000";
    QString const     ocre = "800000";
    QString const     vert = "00cc00";
    QString const     bleu = "0099cc";
    QString           jviu(QString ch);
    QString           ote_diacritiques (QString k);
}
#endif
