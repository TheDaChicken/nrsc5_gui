//
// Created by TheDaChicken on 8/8/2024.
//

#include "DualViewLayout.h"

DualViewLayout::DualViewLayout(QWidget *parent) : QGridLayout(parent)
{
  SetupLayout();
}

void DualViewLayout::SetupLayout()
{
  setSpacing(0);
  setSizeConstraint(SetDefaultConstraint);
  setContentsMargins(0, 0, 0, 0);
}

void DualViewLayout::AddMainWidget(QFrame *widget)
{
  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
  widget->setSizePolicy(sizePolicy1);
  widget->setFrameShape(QFrame::NoFrame);
  widget->setFrameShadow(QFrame::Plain);

  addWidget(widget, 0, 3, 3, 1);
}

void DualViewLayout::AddHeaderWidget(QFrame *widget)
{
  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
  widget->setSizePolicy(sizePolicy2);
  widget->setMaximumSize(QSize(350, 16777215));
  widget->setStyleSheet(QString::fromUtf8("border-bottom-color: transparent;"));
  widget->setFrameShape(QFrame::StyledPanel);
  widget->setFrameShadow(QFrame::Sunken);

  // Added to row: 0 column: 0 row span: 1, column 1
  addWidget(widget, 0, 0, 1, 1);
}

void DualViewLayout::AddLeftWidget(QFrame *widget)
{
  /* Add the list_ */
  QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sizePolicy3.setHorizontalStretch(30);
  sizePolicy3.setVerticalStretch(30);
  sizePolicy3.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
  widget->setSizePolicy(sizePolicy3);
  widget->setMinimumSize(QSize(251, 70));
  widget->setMaximumSize(QSize(350, 16777215));

  /* Set the list style */
  widget->setStyleSheet(QString::fromUtf8("border-top-color: transparent;"));
  widget->setFrameShape(QFrame::StyledPanel);
  widget->setFrameShadow(QFrame::Sunken);

  // Added to row: 1 column: 0 row span: 1, column 1
  addWidget(widget, 1, 0, 1, 1);
}
