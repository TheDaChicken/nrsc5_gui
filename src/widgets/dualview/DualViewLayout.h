//
// Created by TheDaChicken on 8/8/2024.
//

#ifndef NRSC5_GUI_SRC_PAGES_LISTDETAIL_H_
#define NRSC5_GUI_SRC_PAGES_LISTDETAIL_H_

#include <QWidget>
#include <QGridLayout>
#include <QTreeView>

/**
 * Displays TreeView + Main Widget
 */
class DualViewLayout : public QGridLayout
{
 Q_OBJECT
 public:
  explicit DualViewLayout(QWidget *parent = nullptr);

  void AddHeaderWidget(QFrame *widget);
  void AddLeftWidget(QFrame *widget);
  void AddMainWidget(QFrame *widget);

  void AddMainLayout(QLayout *layout);
  void AddHeaderLayout(QLayout *layout);
  void AddLeftLayout(QLayout *layout);
 private:
  void SetupLayout();
};

#endif //NRSC5_GUI_SRC_PAGES_LISTDETAIL_H_
