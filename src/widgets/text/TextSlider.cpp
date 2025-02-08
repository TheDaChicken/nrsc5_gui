//
// Created by TheDaChicken on 12/27/2024.
//

#include "TextSlider.h"

TextSlider::TextSlider(const QString &name, const QString &units,
                       const double min, const double max, QWidget *parent)
	: QWidget(parent),
	  units_(units)
{
	layout_ = new QVBoxLayout(this);
	layout_->setObjectName("TextSliderGroupLayout");
	layout_->setContentsMargins(0, 0, 0, 0);

	slider_layout_ = new QVBoxLayout(this);
	slider_layout_->setObjectName("TextSliderLayout");
	slider_layout_->setContentsMargins(3, 0, 3, 0);

	slider_ = new QSlider(Qt::Horizontal, this);
	slider_->setObjectName("TextSlider");
	slider_->setRange(min, max);
	slider_->setTickInterval(10);
	slider_->setTickPosition(QSlider::TicksBelow);

	slider_layout_->addWidget(slider_);

	text_layout_ = new QHBoxLayout(this);
	text_layout_->setObjectName("TextSliderTextLayout");
	text_layout_->setContentsMargins(0, 0, 0, 0);

	left_label_ = new QLabel(this);
	left_label_->setObjectName("TextSliderLabel");
	left_label_->setAlignment(Qt::AlignLeft);
	left_label_->setText(name);

	right_label_ = new QLabel(this);
	right_label_->setObjectName("TextSliderLabel");
	right_label_->setAlignment(Qt::AlignRight);

	SliderValueChanged(0);

	text_layout_->addWidget(left_label_);
	text_layout_->addStretch();
	text_layout_->addWidget(right_label_);
	text_layout_->setAlignment(left_label_, Qt::AlignLeft);
	text_layout_->setAlignment(right_label_, Qt::AlignRight);

	layout_->addLayout(text_layout_);
	layout_->addLayout(slider_layout_);

	connect(slider_, &QSlider::valueChanged, this, &TextSlider::SliderValueChanged);
}

void TextSlider::SliderValueChanged(const int value)
{
	right_label_->setText(QString("%1 %2").arg(value).arg(units_));
}
