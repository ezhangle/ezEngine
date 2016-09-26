#pragma once

#include <EditorFramework/GUI/RawDocumentTreeModel.moc.h>
#include <ToolsFoundation/Selection/SelectionManager.h>
#include <QTreeView>
#include <memory>
#include <QSortFilterProxyModel>

class EZ_EDITORFRAMEWORK_DLL ezQtDocumentTreeWidget : public QTreeView
{
  Q_OBJECT

public:

  ezQtDocumentTreeWidget(QWidget* pParent);
  ezQtDocumentTreeWidget(QWidget* pParent, ezDocument* pDocument, const ezRTTI* pBaseClass, const char* szChildProperty, std::unique_ptr<ezQtDocumentTreeModel> pCustomModel = nullptr);
  ~ezQtDocumentTreeWidget();

  void Initialize(ezDocument* pDocument, const ezRTTI* pBaseClass, const char* szChildProperty, std::unique_ptr<ezQtDocumentTreeModel> pCustomModel = nullptr);

  void EnsureLastSelectedItemVisible();

  void SetAllowDragDrop(bool bAllow);

  QSortFilterProxyModel* GetProxyFilterModel() const { return m_pFilterModel.get(); }

protected:
  virtual void keyPressEvent(QKeyEvent* e) override;

private slots:
  void on_selectionChanged_triggered(const QItemSelection& selected, const QItemSelection& deselected);

private:
  void SelectionEventHandler(const ezSelectionManagerEvent& e);

private:
  std::unique_ptr<ezQtDocumentTreeModel> m_pModel;
  std::unique_ptr<QSortFilterProxyModel> m_pFilterModel;
  ezDocument* m_pDocument;
  bool m_bBlockSelectionSignal;
};

