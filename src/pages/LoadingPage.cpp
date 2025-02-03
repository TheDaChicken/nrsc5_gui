//
// Created by TheDaChicken on 7/31/2024.
//

#include "LoadingPage.h"

#include <QFrame>

#include <widgets/text/ImageText.h>

LoadingPage::LoadingPage(QWidget *parent) : QWidget(parent)
{
  setObjectName("LoadingLayout");
  resize(1065, 576);

  layout_ = new QHBoxLayout(this);
  layout_->setObjectName("horizontalLayout");
  layout_->setContentsMargins(-1, -1, -1, 100);

  progress_circle_ = new ProgressCircleText(this);
  progress_circle_->setObjectName("ProgressCircle");
  progress_circle_->setMaximumSize(QSize(16777215, 16777215));
  progress_circle_->setFrameShape(QFrame::NoFrame);
  progress_circle_->setFrameShadow(QFrame::Plain);
  progress_circle_->setProperty("circleRadius", QVariant(QSize(100, 100)));
  progress_circle_->setProperty("spacing", QVariant(6));
  progress_circle_->setProperty("innerRadius", QVariant(0.900000000000000));
  progress_circle_->setProperty("outerRadius", QVariant(0.800000000000000));
  progress_circle_->setContentsMargins(-1, 0, -1, 0);
  progress_circle_->setAlignment(
    ImageText::TextBottom |
    ImageText::CenterX |
    ImageText::CenterY
  );

  layout_->addWidget(progress_circle_);
}

LoadingPage::~LoadingPage()
= default;

void LoadingPage::SetStatus(const QString &status, const bool pending) const
{
  progress_circle_->setMoving(pending);
  progress_circle_->setCircleHidden(!pending);
  progress_circle_->setDescription(status);
}
