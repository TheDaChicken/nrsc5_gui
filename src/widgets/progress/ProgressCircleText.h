/////////////////////////////////////////////////////////////////////////////
// Date of creation: 04.07.2013
// Creator: Alexander Egorov aka mofr
// Authors: mofr
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QPropertyAnimation>
#include <utils/Log.h>

#include "widgets/text/ImageTextLabel.h"

/**
 * @brief Widget for showing progress as a circle with a text description
 */
class ProgressCircleText : public ImageTextLabel
{
  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
  Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
  Q_PROPERTY(qreal innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(qreal outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(QColor unfinishedColor READ unfinishedColor WRITE setUnfinishedColor)
  Q_PROPERTY(QSize circleRadius READ circleRadius WRITE setCircleRadius)

  //private
  Q_PROPERTY(qreal infiniteAnimationValue READ infiniteAnimationValue WRITE setInfiniteAnimationValue)
  Q_PROPERTY(int visibleValue READ visibleValue WRITE setVisibleValue)

 public:
  explicit ProgressCircleText(QWidget *parent = nullptr);

  /**
  * @brief value of progress
  * @return
  */
  [[nodiscard]] int value() const;

  /**
   * @brief maximum
   * If maximum <= 0, widget shows infinite process;
   * @return maximum value
   */
  [[nodiscard]] int maximum() const;

  /**
   * @brief innerRadius = [0.0 .. 1.0]
   * @return inner radius value
   */
  [[nodiscard]] qreal innerRadius() const;

  /**
   * @brief outerRadius = [0.0 .. 1.0]
   * @return outer radius value
   */
  [[nodiscard]] qreal outerRadius() const;

  [[nodiscard]] QColor color() const;
  /**
   * @brief color of the unfinished part of the circle
   * @return color
   */
  [[nodiscard]] QColor unfinishedColor() const;

  /**
   * @brief size of provided circle radius.
   * If null, circle will be drawn using size of widget
   * @return
   */
  [[nodiscard]] QSize circleRadius() const;

 public slots:
  /**
   * Allows to stop or start current animation
   * @param start
   */
  void setMoving(bool start);
  void setValue(int value);
  void setCircleHidden(bool hidden);

  /**
   * @brief set maximum value of progress
   * If maximum <= 0, widget shows infinite process;
   * @param maximum
   */
  void setMaximum(int maximum);

  void setInnerRadius(qreal innerRadius);
  void setOuterRadius(qreal outerRadius);
  void setCircleRadius(QSize radius);

  void setUnfinishedColor(QColor color)
  {
   if (color != mUnfinishedColor)
   {
    mUnfinishedColor = color;
    update();
   }
  }

  void setColor(QColor color)
  {
   if (color != mColor)
   {
    mColor = color;
    update();
   }
  }

 signals:
  void valueChanged(int);
  void maximumChanged(int);

 protected:
  void paintEvent(QPaintEvent *) override;

 private slots:
  void setInfiniteAnimationValue(qreal value)
  {
   mInfiniteAnimationValue = value;
   update();
  };

  void setVisibleValue(const int value)
  {
   if (mVisibleValue != value)
   {
    mVisibleValue = value;
    update();
   }
  }

 private:
  [[nodiscard]] QString key() const;
  [[nodiscard]] QPixmap generatePixmap() const;
  [[nodiscard]] qreal infiniteAnimationValue() const;
  [[nodiscard]] int visibleValue() const;

 private:
  bool mHiddenCircle;

  int mValue;
  int mMaximum;
  qreal mInnerRadius;
  qreal mOuterRadius;
  QSize mCircleRadius;
  QColor mColor;
  QColor mUnfinishedColor;

  QPropertyAnimation mValueAnimation;

  int mVisibleValue;
  qreal mInfiniteAnimationValue;
};
