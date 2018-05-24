/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#ifndef LRREPORTDESIGNINTF_P_H
#define LRREPORTDESIGNINTF_P_H

#include <QObject>
#include <QSharedPointer>
#include <QMainWindow>
#include <QLocale>
#include "lrreportengine.h"
#include "lrcollection.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"
#include "lrbanddesignintf.h"
#include "lrreportrender.h"
#include "serializators/lrstorageintf.h"
#include "lrscriptenginemanager.h"
#include "lrreporttranslation.h"
#include "lrdesignerplugininterface.h"
#include "lrreportdesignwindowintrerface.h"

class QFileSystemWatcher;

namespace LimeReport{

class PageDesignIntf;
class PrintRange;
class ReportDesignWindow;

class ReportEnginePrivateInterface {
public:
    virtual PageDesignIntf*         appendPage(const QString& pageName="") = 0;
    virtual bool                    deletePage(PageDesignIntf *page) = 0;
    virtual void                    reorderPages(const QList<PageDesignIntf *> &reorderedPages) = 0;
    virtual int                     pageCount() = 0;
    virtual PageDesignIntf*         pageAt(int index) = 0;
    virtual void                    clearReport() = 0;
    virtual ScriptEngineContext*    scriptContext() = 0;
    virtual ScriptEngineManager*    scriptManager() = 0;
    virtual DataSourceManager*      dataManager() = 0;
    virtual QString                 reportFileName() = 0;
    virtual void                    setReportFileName(const QString& reportFileName) = 0;
    virtual void                    emitSaveFinished() = 0;
    virtual bool                    isNeedToSave() = 0;
    virtual void                    emitSaveReport() = 0;
    virtual bool                    saveToFile(const QString& fileName = "") = 0;
    virtual bool                    isSaved() = 0;
    virtual QString                 reportName() = 0;
    virtual bool                    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange) = 0;
    virtual bool                    emitLoadReport() = 0;
    virtual void                    clearSelection() = 0;
    virtual bool                    printReport(QPrinter *printer=0) = 0;
    virtual void                    previewReport(PreviewHints hints = PreviewBarsUserSetting) = 0;
    virtual void                    setCurrentReportsDir(const QString& dirName) = 0;
    virtual QString                 currentReportsDir() = 0;
    virtual bool                    suppressFieldAndVarError() const = 0;
    virtual void                    setSuppressFieldAndVarError(bool suppressFieldAndVarError) = 0;
    virtual void                    setStyleSheet(const QString& styleSheet) = 0;
    virtual QString                 styleSheet() const = 0;
    virtual QList<QLocale::Language> designerLanguages() = 0;
    virtual QLocale::Language       currentDesignerLanguage() = 0;
    virtual void                    setCurrentDesignerLanguage(QLocale::Language language) = 0;
};

class ReportEnginePrivate : public QObject, public ICollectionContainer, public ITranslationContainer,
        public ReportEnginePrivateInterface
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ReportEngine)
    Q_PROPERTY(ACollectionProperty pages READ fakeCollectionReader())
    Q_PROPERTY(QObject* datasourcesManager READ dataManager)
    Q_PROPERTY(QObject* scriptContext READ scriptContext)
    Q_PROPERTY(bool suppressFieldAndVarError READ suppressFieldAndVarError WRITE setSuppressFieldAndVarError)
    Q_PROPERTY(ATranslationProperty translation READ fakeTranslationReader)

    friend class PreviewReportWidget;
public:
    static void printReport(ItemsReaderIntf::Ptr reader, QPrinter &printer);
    static void printReport(ReportPages pages, QPrinter &printer);
    Q_INVOKABLE QStringList aviableReportTranslations();
    Q_INVOKABLE void setReportTranslation(const QString& languageName);
public:
    explicit ReportEnginePrivate(QObject *parent = 0);
    virtual ~ReportEnginePrivate();

    PageDesignIntf*      appendPage(const QString& pageName="");
    bool deletePage(PageDesignIntf *page);
    PageDesignIntf*      createPreviewPage();
    PageDesignIntf*      pageAt(int index){return (index<=(m_pages.count()-1)) ? m_pages.at(index):0;}
    int                  pageCount() {return m_pages.count();}
    DataSourceManager*   dataManager(){return m_datasources;}
    ScriptEngineContext* scriptContext(){return m_scriptEngineContext;}
    ScriptEngineManager* scriptManager();
    IDataSourceManager*  dataManagerIntf(){return m_datasources;}

    IScriptEngineManager* scriptManagerIntf(){
        ScriptEngineManager::instance().setDataManager(dataManager());
        return &ScriptEngineManager::instance();
    }

    void    clearReport();
    bool    printReport(QPrinter *printer=0);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    bool    printToPDF(const QString& fileName);
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);

    ReportDesignWindowInterface* getDesignerWindow();
    void    designReport();
    void    setSettings(QSettings* value);
    void    setShowProgressDialog(bool value){m_showProgressDialog = value;}
    QSettings*  settings();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange);
    bool    loadFromByteArray(QByteArray *data, const QString& name = "");
    bool    loadFromString(const QString& report, const QString& name = "");
    QString reportFileName(){return m_fileName;}
    void    setReportFileName(const QString& reportFileName){ m_fileName = reportFileName;}
    bool    saveToFile(const QString& fileName = "");
    QByteArray  saveToByteArray();
    QString saveToString();
    bool    isNeedToSave();
    QString lastError();
    ReportEngine * q_ptr;
    void emitSaveReport();
    bool emitLoadReport();
    void emitSaveFinished();
    void emitPrintedToPDF(QString fileName);
    bool isSaved();
    void setCurrentReportsDir(const QString& dirName);
    QString currentReportsDir(){ return m_reportsDir;}
    void setReportName(const QString& reportName){ m_reportName=reportName;}
    QString reportName(){ return m_reportName;}
    bool hasActivePreview(){return m_activePreview;}
    PageDesignIntf *createPreviewScene(QObject *parent);
    PreviewReportWidget *createPreviewWidget(QWidget *parent);
    QIcon previewWindowIcon() const;
    void setPreviewWindowIcon(const QIcon &previewWindowIcon);
    QString previewWindowTitle() const;
    void setPreviewWindowTitle(const QString &previewWindowTitle);

    bool suppressFieldAndVarError() const;
    void setSuppressFieldAndVarError(bool suppressFieldAndVarError);
    bool isBusy();
    bool resultIsEditable() const;
    void setResultEditable(bool value);

    void setPassPhrase(const QString &passPhrase);
    bool addTranslationLanguage(QLocale::Language language);
    bool removeTranslationLanguage(QLocale::Language language);
    bool setReportLanguage(QLocale::Language language);
    QList<QLocale::Language> aviableLanguages();
    ReportTranslation* reportTranslation(QLocale::Language language);
    void reorderPages(const QList<PageDesignIntf *> &reorderedPages);
    void clearSelection();
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
    QString styleSheet() const;
    void setStyleSheet(const QString &styleSheet);
    QList<QLocale::Language> designerLanguages();
    QLocale::Language currentDesignerLanguage();
    void setCurrentDesignerLanguage(QLocale::Language language);
signals:
    void    pagesLoadFinished();
    void    datasourceCollectionLoadFinished(const QString& collectionName);
    void    cleared();
    void    renderStarted();
    void    renderFinished();
    void    renderPageFinished(int renderedPageCount);
    void    onLoad(bool& loaded);
    void    onSave();
    void    saveFinished();

    void    loaded();
    void    printedToPDF(QString fileName);

    void    getAviableLanguages(QList<QLocale::Language>* languages);
    void    currentDefaulLanguageChanged(QLocale::Language);
    QLocale::Language  getCurrentDefaultLanguage();

public slots:
    bool    slotLoadFromFile(const QString& fileName);
    void    cancelRender();
protected:
    PageDesignIntf* createPage(const QString& pageName="");
protected slots:
    void    slotDataSourceCollectionLoaded(const QString& collectionName);
private slots:
    void slotPreviewWindowDestroyed(QObject* window);
    void slotDesignerWindowDestroyed(QObject* window);
private:
    //ICollectionContainer
    virtual QObject*    createElement(const QString&,const QString&);
    virtual int         elementsCount(const QString&);
    virtual QObject*    elementAt(const QString&, int index);
    virtual void        collectionLoadFinished(const QString&);
    void    saveError(QString message);
    void    showError(QString message);
    //ICollectionContainer
    //ITranslationContainer
    Translations* translations(){ return &m_translations;}
    void updateTranslations();
    //ITranslationContainer
    ReportPages renderToPages();
    QString renderToString();
    PageItemDesignIntf *getPageByName(const QString& pageName);
    ATranslationProperty fakeTranslationReader(){ return ATranslationProperty();}
    PageItemDesignIntf *createRenderingPage(PageItemDesignIntf *page);
private:
    QList<PageDesignIntf*> m_pages;
    QList<PageItemDesignIntf*> m_renderingPages;
    DataSourceManager* m_datasources;
    ScriptEngineContext* m_scriptEngineContext;
    ReportRender::Ptr m_reportRender;
    QString m_fileName;
    QString m_lastError;
    QSettings* m_settings;
    bool m_ownedSettings;
    QScopedPointer<QPrinter> m_printer;
    bool m_printerSelected;
    bool m_showProgressDialog;
    QString m_reportsDir;
    QString m_reportName;
    QMainWindow* m_activePreview;
    QIcon m_previewWindowIcon;
    QString m_previewWindowTitle;
    QPointer<ReportDesignWindowInterface> m_designerWindow;
    ReportSettings m_reportSettings;
    bool m_reportRendering;
    bool m_resultIsEditable;
    QString m_passPhrase;
    QFileSystemWatcher  *m_fileWatcher;
    Translations m_translations;
    QLocale::Language m_reportLanguage;
    void activateLanguage(QLocale::Language language);
    Qt::LayoutDirection m_previewLayoutDirection;
    LimeReportPluginInterface* m_designerFactory;
    QString m_styleSheet;
    QLocale::Language m_currentDesignerLanguage;
};

}
#endif // LRREPORTDESIGNINTF_P_H
