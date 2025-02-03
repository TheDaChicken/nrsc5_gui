//
// Created by TheDaChicken on 8/31/2024.
//

#ifndef NRSC5_GUI_SRC_THEMES_THEMEMANAGER_H_
#define NRSC5_GUI_SRC_THEMES_THEMEMANAGER_H_

#include <vector>
#include <QString>
#include <QSet>
#include <QMap>
#include <QPalette>
#include <QVariant>
#include <utils/Log.h>

struct ThemeVar
{
	enum Type
	{
		Color,
		Alias,
		String,
	};

	QString name;
	QVariant value;
	Type type;
};

struct ThemeData
{
	QString name;
	QString content;
	qsizetype headerEnd;

	QHash<QString, ThemeVar> vars;
	QString dependency;

	[[nodiscard]] QString Content() const
	{
		return content.mid(headerEnd);
	}

	void ParseTheme();

	static ThemeVar ParseVar(const QString &key, const QString &value);
	static std::tuple<QString, QString> ParseLine(QString &line);
	static QString StripStrValue(const QString &value);
};

class ThemeManager
{
	public:
		void LoadThemes();

		ThemeData GetTheme(QStringView name) const;
		ThemeData GetCurrentTheme() const
		{
			return GetTheme(currentTheme);
		}

		QVariant GetValue(const QString &key, QVariant defaultValue = QVariant()) const;
		void SetTheme(QStringView name);
	private:
		static void ResolveAliases(const QHash<QString, ThemeVar> &vars, ThemeVar &themeVar);
		static QString PrepareQSS(QString content, const QHash<QString, ThemeVar> &vars);
		[[nodiscard]] QPalette PreparePalette(const QHash<QString, ThemeVar> &vars) const;

		std::vector<ThemeData> themes;
		QString currentTheme;
		QPalette defaultPalette;
};

#endif //NRSC5_GUI_SRC_THEMES_THEMEMANAGER_H_
