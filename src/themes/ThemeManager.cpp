//
// Created by TheDaChicken on 8/31/2024.
//

#include "ThemeManager.h"
#include "utils/TokenParser.h"
#include "utils/Log.h"

#include <QDirIterator>
#include <QApplication>
#include <utility>
#include <QMetaEnum>

template<typename T>
static void FillEnumMap(QHash<QString, T> &map)
{
	const QMetaEnum meta = QMetaEnum::fromType<T>();
	const int numKeys = meta.keyCount();

	for (int i = 0; i < numKeys; i++)
	{
		const char *key = meta.key(i);
		QString keyName(key);
		map[keyName.toLower()] = static_cast<T>(meta.keyToValue(key));
	}
}

/**
 * Parses a line of a theme.
 * Example:
 * background: #000;
 * test: 'test';
 * @param line
 * @return
 */
std::tuple<QString, QString> ThemeData::ParseLine(QString &line)
{
	TokenParser lineParser(line, QList<QString>{":", " "});
	QString key = lineParser.GetToken();
	QString value = lineParser.GetToken();

	if (!value.endsWith(";"))
	{
		Logger::Log(err, "Invalid line: {} value: {}", line, value);
		return {};
	}

	value = value.left(value.length() - 1);
	return std::make_tuple(key, value);
}

/**
 * Value must be a string.
 * @param value
 * @return
 */
QString ThemeData::StripStrValue(const QString &value)
{
	if (value.startsWith("\"") || value.startsWith("'"))
	{
		return value.mid(1, value.length() - 2);
	}

	Logger::Log(err, "Unknown value: {}", value.toStdString());
	return {};
}

/**
 * Parses a variable value.
 * Example:
 * rgb(0, 0, 0, 0);
 * var(--blue1);
 *
 * @param key The variable name
 */
ThemeVar ThemeData::ParseVar(const QString &key, const QString &value)
{
	if (value.startsWith("var(--"))
	{
		const QString varName = value.mid(6, value.length() - 7);
		return {key, varName, ThemeVar::Alias};
	}
	if (value.startsWith("rgb(") && value.endsWith(")"))
	{
		QStringList parts = value.mid(4, value.length() - 5).split(",");
		if (parts.length() < 3 && parts.length() > 4)
		{
			Logger::Log(err, "Invalid rgb value: {}", value.toStdString());
			return {};
		}

		QColor color(parts[0].toInt(), parts[1].toInt(), parts[2].toInt());

		// alpha channel can be optional
		if (parts.length() > 3)
		{
			color.setAlpha(parts[3].toInt());
		}

		return {key, color, ThemeVar::Color};
	}
	if (value.startsWith("#"))
	{
		return {key, QColor(value), ThemeVar::Color};
	}
	if (value.startsWith("\"") || value.startsWith("'"))
	{
		// Remove quotes
		return {key, value.mid(1, value.length() - 2), ThemeVar::String};
	}

	return {key, value, ThemeVar::String};
}

/*
 * Parses a theme header.
 * Extracts the variables and dependencies.
 * CSS like syntax is used.
 *
 * Example:
 * @ThemeMeta {
 *  name: "Dark";
 * }
 * @ThemeVars {
 * 	 --background: #000;
 * }
 */
void ThemeData::ParseTheme()
{
	TokenParser parser(content, QList<QString>{"\n", "\r", "\t"});
	qsizetype pos;

	while (parser.HasNext())
	{
		pos = parser.GetPos();

		QString header = parser.GetToken();
		if (!header.startsWith("@"))
		{
			break;
		}

		if (QString headerName = header.mid(1, header.indexOf(" ") - 1); headerName == "ThemeMeta")
		{
			while (parser.HasNext())
			{
				QString line = parser.GetToken().simplified();
				if (line.startsWith("}"))
				{
					break;
				}

				auto [key, value] = ParseLine(line);

				value = StripStrValue(value);
				if (key == "name")
				{
					name = value;
				}
				else if (key == "dependencies")
				{
					dependency = value;
				}
			}
		}
		else if (headerName == "ThemeVars")
		{
			while (parser.HasNext())
			{
				QString line = parser.GetToken().simplified();
				if (line.startsWith("}"))
				{
					break;
				}

				// Ignore comments
				if (line.startsWith("/*"))
				{
					continue;
				}

				// Variable line starts with --
				if (!line.startsWith("--"))
				{
					Logger::Log(err, "Invalid variable: {}", line.toStdString());
					continue;
				}

				line = line.mid(2);

				auto [key, value] = ParseLine(line);
				if (key.isEmpty() && value.isEmpty())
				{
					continue;
				}

				vars[key] = ParseVar(key, value);
			}
		}
		else
		{
			Logger::Log(err, "Unknown theme header: {}", headerName);
		}
	}

	headerEnd = pos;
}

void ThemeManager::LoadThemes()
{
	defaultPalette = qApp->palette();

	QDirIterator it(QString::fromStdString(":/themes"), QDir::Files);

	while (it.hasNext())
	{
		QString themePath = it.next();
		QFile file(themePath);
		if (!file.open(QIODevice::ReadOnly))
		{
			continue;
		}

		ThemeData theme;
		theme.content = file.readAll();
		theme.ParseTheme();

		themes.push_back(theme);
	}
}

ThemeData ThemeManager::GetTheme(const QStringView name) const
{
	for (const ThemeData &theme : themes)
	{
		if (theme.name == name)
		{
			return theme;
		}
	}

	return {};
}

void ThemeManager::ResolveAliases(const QHash<QString, ThemeVar> &vars, ThemeVar &themeVar)
{
	while (themeVar.type == ThemeVar::Alias)
	{
		QString alias = themeVar.value.toString();

		if (!vars.contains(alias))
		{
			Logger::Log(err, "Unknown variable: {}", alias.toStdString());
			break;
		}

		themeVar = vars[alias];
	}
}

QString ThemeManager::PrepareQSS(QString content, const QHash<QString, ThemeVar> &vars)
{
	const auto needle = QString("var(--%1)");
	QString qss = std::move(content);

	// Replace variables with their values
	for (const auto &[key, value] : vars.asKeyValueRange())
	{
		ThemeVar themeVar = value;

		// Resolve alias
		ResolveAliases(vars, themeVar);

		QString variableNeedle = needle.arg(key);

		// Resolve variable
		switch (themeVar.type)
		{
			case ThemeVar::Color:
			{
				auto color = themeVar.value.value<QColor>();
				qss.replace(variableNeedle, color.name());
				break;
			}
			case ThemeVar::String:
			{
				qss.replace(variableNeedle, themeVar.value.toString());
				break;
			}
			default:
			{
				Logger::Log(err,
				            "Unknown variable type: {} {}",
				            static_cast<int>(themeVar.type),
				            key);
				break;
			}
		}
	}

	return qss;
}

QPalette ThemeManager::PreparePalette(const QHash<QString, ThemeVar> &vars) const
{
	QPalette palette = defaultPalette;

	static QHash<QString, QPalette::ColorRole> roleMap;
	static QHash<QString, QPalette::ColorGroup> groupMap;

	if (roleMap.empty())
		FillEnumMap<QPalette::ColorRole>(roleMap);
	if (groupMap.empty())
		FillEnumMap<QPalette::ColorGroup>(groupMap);

	for (const auto &[key, value] : vars.asKeyValueRange())
	{
		ThemeVar themeVar = value;
		if (!key.startsWith("palette_"))
		{
			continue;
		}

		// Resolve alias
		ResolveAliases(vars, themeVar);

		// Format: palette_<role>[_group]
		QPalette::ColorRole role = QPalette::NoRole;
		QPalette::ColorGroup group = QPalette::All;

		TokenParser parser(key, "_");

		if (const QString prefix = parser.GetToken().toLower(); prefix != "palette")
		{
			continue;
		}

		QString name = parser.GetToken().toLower();

		// Has role
		if (!roleMap.contains(name))
		{
			Logger::Log(err, "Unknown palette role: {}", name.toStdString());
			continue;
		}

		role = roleMap[name];
		QColor color = themeVar.value.value<QColor>().name(QColor::HexRgb);

		// Has group
		if (parser.HasNext())
		{
			if (QString groupStr = parser.GetToken().toLower(); groupMap.contains(groupStr))
			{
				group = groupMap[groupStr];
			}
		}

		palette.setColor(group, role, color);
	}

	return palette;
}

QVariant ThemeManager::GetValue(const QString &key, QVariant defaultValue) const
{
	const ThemeData theme = GetCurrentTheme();

	if (theme.vars.contains(key))
	{
		return theme.vars[key].value;
	}

	const ThemeData defaultTheme = GetTheme(theme.dependency);
	if (defaultTheme.vars.contains(key))
	{
		return defaultTheme.vars[key].value;
	}

	return defaultValue;
}

void ThemeManager::SetTheme(const QStringView name)
{
	ThemeData theme = GetTheme(name);
	QHash<QString, ThemeVar> vars;
	QString content;

	if (theme.name.isEmpty())
	{
		Logger::Log(err, "Theme doesn't exist");
		return;
	}

	// Include content from dependencies
	if (!theme.dependency.isEmpty())
	{
		ThemeData dependency = GetTheme(theme.dependency);
		if (dependency.name.isEmpty())
		{
			Logger::Log(err, "Dependency doesn't exist {}", theme.dependency);
			return;
		}

		content += dependency.Content();

		for (const auto &[key, value] : dependency.vars.asKeyValueRange())
		{
			vars[key] = value;
		}
	}

	content += theme.Content();

	// Include content from the theme
	for (const auto &[key, value] : theme.vars.asKeyValueRange())
	{
		vars[key] = value;
	}

	qApp->setStyleSheet(PrepareQSS(content, vars));
	qApp->setPalette(PreparePalette(vars));

#ifdef _DEBUG
	// Debug testing purposes
	QString filename;
	filename += name;
	filename += ".qss.output";
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(qApp->styleSheet().toUtf8());
	}
	file.close();
#endif

	currentTheme = name.toString();

	Logger::Log(info,
	            "Theme set to: {}",
	            std::string_view(name.toUtf8().constData(), name.size()));
}
