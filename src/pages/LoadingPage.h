//
// Created by TheDaChicken on 7/31/2024.
//

#ifndef NRSC5_GUI_SRC_PAGES_LOADINGSTATUS_H_
#define NRSC5_GUI_SRC_PAGES_LOADINGSTATUS_H_

#include <QWidget>
#include <QHBoxLayout>

#include "widgets/progress/ProgressCircleText.h"

class LoadingPage : public QWidget
{
		Q_OBJECT

	public:
		explicit LoadingPage(QWidget *parent = nullptr);
		~LoadingPage() override;

		void SetStatus(const QString &status, bool isLoading = true) const;

	private:
		QHBoxLayout *layout_;
		ProgressCircleText *progress_circle_;
};

#endif //NRSC5_GUI_SRC_PAGES_LOADINGSTATUS_H_
