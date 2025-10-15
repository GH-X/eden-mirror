#ifndef GAMELISTMODEL_H
#define GAMELISTMODEL_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QStandardItemModel>
#include "Interface/QMLConfig.h"
#include "qt_common/config/uisettings.h"
#include <core/hle/service/am/applet_manager.h>

typedef struct Game {
    QString absPath;
    QString name;
    QString fileSize;
} Game;

class GameListWorker;
class GameIconProvider;

class GameListModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum GLMRoleTypes {
        NAME = Qt::UserRole + 1,
        PATH,
        FILESIZE,
        ICON
    };

    enum class StartGameType {
        Normal, // Can use custom configuration
        Global, // Only uses global configuration
    };

    GameListModel(QObject *parent, QQmlEngine *engine, QMLConfig *config);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addEntry(QStandardItem *entry, const UISettings::GameDir &parent_dir);
    void addDirEntry(const UISettings::GameDir &dir);
    void donePopulating(QStringList watch_list);
    void populateAsync(QVector<UISettings::GameDir> &game_dirs);

    void RefreshGameDirectory();

    void BootGame(const QString &filename,
                  Service::AM::FrontendAppletParameters params,
                  StartGameType type);
    bool LoadROM(const QString &filename, Service::AM::FrontendAppletParameters params);
private slots:
    void WorkerEvent();

private:
    QStringList m_dirs;
    QList<Game> m_data;
    QFileSystemWatcher *watcher = nullptr;
    std::unique_ptr<GameListWorker> current_worker;

    GameIconProvider *m_provider;

    QMLConfig *m_config;
};

#endif // GAMELISTMODEL_H
