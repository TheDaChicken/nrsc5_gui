//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STATIONDELEGATE_H
#define NRSC5_GUI_STATIONDELEGATE_H

#include <QPixmapCache>
#include <QStyledItemDelegate>

/**
 * @brief Delegate for rendering channel items in a list.
 */
class ChannelDelegate : public QStyledItemDelegate
{
  Q_OBJECT

 public:
  constexpr static int kChannelIconMargin = 12;
  constexpr static int kChannelMargin = 10;
  constexpr static int kChannelBoxTextMargin = 6;

  constexpr static QSize kChannelIconSize = QSize(50, 50);
  constexpr static QSize kChannelBoxIconSize = QSize(64, 64);

  constexpr static QSize kChannelDeleteSize = QSize(24, 24);

  enum Styles
  {
   kDefault,
   kBoxLayout
  };

  explicit ChannelDelegate(QObject *parent = nullptr, const Styles style = kDefault)
   : QStyledItemDelegate(parent), style_(style)
  {
  }

  ~ChannelDelegate() override = default;

  Q_DISABLE_COPY(ChannelDelegate)

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

 protected:
  bool editorEvent(QEvent *event, QAbstractItemModel *model,
                   const QStyleOptionViewItem &option, const QModelIndex &index) override;

 private:
  QPixmap ReadIcon(const QVariant &icon) const;
  QRect CalculateContentRect(const QRect &rect) const;

  [[nodiscard]] QSize MaxIconSize(const QRect &contentRect) const;
  [[nodiscard]] QRect CalculateDeleteButtonRect(const QRect &contentRect) const;
  [[nodiscard]] QRectF CalculateIconRect(const QRect &contentRect, QSizeF iconSize) const;
  [[nodiscard]] QRectF CalculateTextRect(const QRect &contentRect, const QRectF &iconRect) const;

  Styles style_ = kDefault;

  /**
   * @brief Scales the pixmap to the specified size and caches the result.
   * @param icon The icon to scale.
   * @param size The size to scale the icon to.
   * @param deviceRatio
   * @return The scaled icon.
   */
  static QPixmap CacheScaleIcon(const QPixmap &icon, const QSize &size, qreal deviceRatio);
};

#endif //NRSC5_GUI_STATIONDELEGATE_H
