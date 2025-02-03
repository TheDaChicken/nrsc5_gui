//
// Created by TheDaChicken on 9/9/2024.
//

#ifndef BASICTEXTDELEGATE_H
#define BASICTEXTDELEGATE_H

#include <QStyledItemDelegate>

/**
* This is a simple delegate that draws text.
* This is meant to match the same margins as Channel Delegate.
*/
class BasicTextDelegate : public QStyledItemDelegate
{
		Q_OBJECT

	public:
		static constexpr int kTextMargin = 10;

		explicit BasicTextDelegate(QObject *parent = nullptr)
			: QStyledItemDelegate(parent)
		{
		}

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
		[[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif //BASICTEXTDELEGATE_H
