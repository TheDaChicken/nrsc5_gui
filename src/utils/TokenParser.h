//
// Created by TheDaChicken on 8/31/2024.
//

#ifndef NRSC5_GUI_SRC_UTILS_TOKENPARSER_H_
#define NRSC5_GUI_SRC_UTILS_TOKENPARSER_H_

#include <QString>
#include <QList>
#include <functional>
#include <utility>
#include "utils/Log.h"

/**
 * Acts similar to a Java Scanner class.
 */
class TokenParser
{
 public:
  TokenParser(QString content, const QString& delim)
	  : content_(std::move(content)), pos(0)
  {
	delims_.append(delim);
  }

  TokenParser(QString content, QList<QString> delims)
	  : pos(0), content_(std::move(content)), delims_(std::move(delims))
  {

  }

  ~TokenParser() = default;

  [[nodiscard]] bool HasNext() const
  {
	return pos < content_.length();
  }

  [[nodiscard]] qsizetype GetPos() const
  {
	return pos;
  }

  /**
   * Gets the next token. If there are no more tokens, an empty string is returned.
   * Example: test\ntest\n\n4 -> test -> test -> 4 -> ""
   * @return
   */
  QString GetToken()
  {
	if (pos >= content_.length())
	{
	  return "";
	}

	// Skip delimiters
	QString token = "";

	while(pos < content_.length())
	{
	  QString character = content_[pos];

	  if (delims_.contains(character))
	  {
		pos++;

		if(token.isEmpty())
		{
		  continue;
		}
		return token;
	  }

	  pos++;
	  token += character;
	}

	return token;
  }
 private:
  QString content_;
  QList<QString> delims_; // Delimiters to split the content
  qsizetype pos;
};

#endif //NRSC5_GUI_SRC_UTILS_TOKENPARSER_H_
