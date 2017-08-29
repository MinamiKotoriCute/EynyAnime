#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>

#include "zippysharewebcontrol.h"
#include "animeinfo.h"
#include "treewidget.h"

class DownloadWidget : public QWidget
{
    Q_OBJECT
public:
    static DownloadWidget& Instance();

    void addAnime(QString eynyUrl);
    void addAnime(AnimeInfo animeInfo);

signals:

public slots:

protected:
    void closeEvent(QCloseEvent *);

private:
    explicit DownloadWidget(QWidget *parent = 0);
    ~DownloadWidget();

    void readSettings();
    void writeSettings();

    TreeWidget *treeWidget;
    QString username, password;
    QString baseDir;
    Downloader downloader;

    QMap<QTreeWidgetItem *, AnimeInfo> mapToAnimeInfo;
};

#endif // DOWNLOADWIDGET_H
