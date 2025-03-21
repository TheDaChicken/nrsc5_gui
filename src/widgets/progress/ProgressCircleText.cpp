/////////////////////////////////////////////////////////////////////////////
// Date of creation: 04.07.2013
// Creator: Alexander Egorov aka mofr
// Authors: mofr
/////////////////////////////////////////////////////////////////////////////

#include "ProgressCircleText.h"
#include "utils/Log.h"

#include <QPainter>
#include <QPixmapCache>
#include <QStyle>

ProgressCircleText::ProgressCircleText(QWidget *parent)
	: ImageTextLabel(parent),
	  mHiddenCircle(false),
	  mValue(0),
	  mMaximum(-1),
	  mInnerRadius(0.6),
	  mOuterRadius(1.0),
	  mColor(110, 190, 235),
	  mUnfinishedColor(225, 225, 225),
	  mValueAnimation(this, "visibleValue"),
	  mVisibleValue(0),
	  mInfiniteAnimationValue(0)
{
	// Set up animation
	mValueAnimation.setStartValue(0.0);
	mValueAnimation.setEndValue(1.0);

	// Start infinite animation
	setMaximum(0);
	// SetupLayout pixmap for widget sizing
	setPixmap(generatePixmap());
}

int ProgressCircleText::value() const
{
	return mValue;
}

int ProgressCircleText::maximum() const
{
	return mMaximum;
}

qreal ProgressCircleText::innerRadius() const
{
	return mInnerRadius;
}

qreal ProgressCircleText::outerRadius() const
{
	return mOuterRadius;
}

QColor ProgressCircleText::color() const
{
	return mColor;
}

void ProgressCircleText::setValue(int value)
{
	if (value < 0) value = 0;

	if (mValue != value)
	{
		mValue = value;
		emit valueChanged(value);

		// Reset animation to show progress
		setMoving(false);
		setMoving(true);
	}
}

void ProgressCircleText::setMaximum(int maximum)
{
	if (maximum < 0) maximum = 0;

	if (mMaximum != maximum)
	{
		mMaximum = maximum;
		update();
		emit maximumChanged(maximum);

		// Start infinite animation
		setMoving(false);
		setMoving(true);
	}
}

void ProgressCircleText::setInnerRadius(qreal innerRadius)
{
	if (innerRadius > 1.0) innerRadius = 1.0;
	if (innerRadius < 0.0) innerRadius = 0.0;

	if (mInnerRadius != innerRadius)
	{
		mInnerRadius = innerRadius;
		update();
	}
}

void ProgressCircleText::setOuterRadius(qreal outerRadius)
{
	if (outerRadius > 1.0) outerRadius = 1.0;
	if (outerRadius < 0.0) outerRadius = 0.0;

	if (mOuterRadius != outerRadius)
	{
		mOuterRadius = outerRadius;
		update();
	}
}

void ProgressCircleText::paintEvent(QPaintEvent *)
{
	const QRect contentRect = contentsRect();
	QPainter painter(this);

	if (mHiddenCircle)
	{
		// Empty circle
		DrawItems(painter, contentRect, {});
		return;
	}

	QPixmap pixmap;
	if (!QPixmapCache::find(key(), &pixmap))
	{
		pixmap = generatePixmap();
		QPixmapCache::insert(key(), pixmap);
	}

	DrawItems(painter, contentRect, pixmap);
}

void ProgressCircleText::setCircleHidden(const bool hidden)
{
	mHiddenCircle = hidden;
	update();
}

void ProgressCircleText::setMoving(const bool start)
{
	if (mValueAnimation.state() == QAbstractAnimation::Running && start)
		return;

	if (start)
	{
		if (mMaximum == 0)
		{
			mValueAnimation.setPropertyName("infiniteAnimationValue");
			mValueAnimation.setLoopCount(-1);
			mValueAnimation.setEndValue(1.0);
			mValueAnimation.setDuration(1000);
			mValueAnimation.start();
		}
		else
		{
			mValueAnimation.setPropertyName("visibleValue");
			mValueAnimation.setLoopCount(0);
			mValueAnimation.setEndValue(mValue);
			mValueAnimation.setDuration(250);
			mValueAnimation.start();
		}
	}
	else
	{
		mValueAnimation.stop();
	}
}

void ProgressCircleText::setCircleRadius(const QSize radius)
{
	mCircleRadius = radius;
	update();
}

QRectF squared(const QRectF &rect)
{
	if (rect.width() > rect.height())
	{
		const qreal diff = rect.width() - rect.height();
		return rect.adjusted(diff / 2, 0, -diff / 2, 0);
	}
	else
	{
		const qreal diff = rect.height() - rect.width();
		return rect.adjusted(0, diff / 2, 0, -diff / 2);
	}
}

QPixmap ProgressCircleText::generatePixmap() const
{
	QPixmap pixmap;

	if (mCircleRadius.isValid())
	{
		pixmap = QPixmap(mCircleRadius);
	}
	else
	{
		pixmap = QPixmap(squared(rect()).size().toSize());
	}

	pixmap.fill(QColor(0, 0, 0, 0));

	QPainter painter(&pixmap);

	painter.setRenderHint(QPainter::Antialiasing, true);

	QRectF rect = pixmap.rect().adjusted(1, 1, -1, -1);
	const qreal margin = rect.width() * (1.0 - mOuterRadius) / 2.0;
	rect.adjust(margin, margin, -margin, -margin);
	const qreal innerRadius = mInnerRadius * rect.width() / 2.0;

	//background grey circle
	painter.setBrush(QColor(mUnfinishedColor));
	painter.setPen(QColor(mUnfinishedColor));
	painter.drawPie(rect, 0, 360 * 16);

	painter.setBrush(mColor);
	painter.setPen(mColor);

	if (mMaximum <= 0)
	{
		//draw as infinite process
		constexpr int spanAngle = 0.15 * 360 * 16;
		const int startAngle = qCeil(-mInfiniteAnimationValue * 360 * 16);

		painter.drawPie(rect, startAngle, spanAngle);
	}
	else
	{
		constexpr int startAngle = 90 * 16;
		const int value = qMin(mVisibleValue, mMaximum);
		const int spanAngle = qCeil(-static_cast<qreal>(value) * 360 * 16.0 / mMaximum);

		painter.drawPie(rect, startAngle, spanAngle);
	}

	//inner circle and frame
	painter.setCompositionMode(QPainter::CompositionMode_Clear);
	painter.setBrush(palette().color(backgroundRole()));
	painter.setPen(QColor(0, 0, 0, 60));
	painter.drawEllipse(rect.center(), innerRadius, innerRadius);

	//outer frame
	painter.drawArc(rect, 0, 360 * 16);
	return pixmap;
}

QString ProgressCircleText::key() const
{
	return QString("%1,%2,%3,%4,%5,%6,%7,%8")
			// animation value
			.arg(mInfiniteAnimationValue)
			.arg(mVisibleValue)
			// other values
			.arg(mMaximum)
			.arg(mInnerRadius)
			.arg(mOuterRadius)
			.arg(width())
			.arg(height())
			.arg(mColor.rgb());
}

qreal ProgressCircleText::infiniteAnimationValue() const
{
	return mValue;
}

int ProgressCircleText::visibleValue() const
{
	return mVisibleValue;
}

QSize ProgressCircleText::circleRadius() const
{
	return mCircleRadius;
}

QColor ProgressCircleText::unfinishedColor() const
{
	return mUnfinishedColor;
}
