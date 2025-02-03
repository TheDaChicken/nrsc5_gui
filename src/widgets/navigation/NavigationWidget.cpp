//
// Created by TheDaChicken on 8/15/2024.
//

#include "NavigationWidget.h"
#include "utils/Log.h"
#include "widgets/text/ImageTextLabel.h"

#include <QHBoxLayout>
#include <QPushButton>

NavigationWidget::NavigationWidget(QWidget *parent) : QFrame(parent)
{
  setObjectName("NavigationWidget");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout_ = new QHBoxLayout(this);
  layout_->setContentsMargins(30, 11, 30, 7);
  layout_->setSpacing(20);
  layout_->setSizeConstraint(QLayout::SetMinimumSize);

  layout_->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  image_label_ = new ImageTextLabel(this);
  image_label_->setObjectName("ViewPicture");
  image_label_->setAlignment(ImageText::ImageCenterY);
  image_label_->setPixmap(QPixmap(":/buttons/BlueBack.svg"));
  image_label_->setMinimumSize({15, 15});
  image_label_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  layout_->addWidget(image_label_);

  // Add name of the current view
  view_label_ = new NavigationHeader(this);
  view_label_->setObjectName("ViewName");
  view_label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  view_label_->setMinimumSize({15, 15});
  view_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  view_label_->setTextFormat(Qt::PlainText);
  view_label_->setText(tr("Radio"));

  layout_->addWidget(view_label_);
  layout_->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  layout_->setStretchFactor(image_label_, 1);
  layout_->setStretchFactor(view_label_, 2);
}

void NavigationWidget::mousePressEvent(QMouseEvent *event)
{
  QWidget::mousePressEvent(event);
}
