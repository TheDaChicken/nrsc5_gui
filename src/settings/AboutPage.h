//
// Created by TheDaChicken on 10/13/2024.
//

#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class AboutPage : public QWidget
{
		Q_OBJECT

	public:
		explicit AboutPage(QWidget *parent = nullptr);
		~AboutPage() override;

	private:
		QVBoxLayout *layout;
		QTextEdit *text;
};

#endif //ABOUTPAGE_H
