//
// Created by TheDaChicken on 9/8/2024.
//

#ifndef NAVIGATIONHEADER_H
#define NAVIGATIONHEADER_H

#include "widgets/CapsLabelAlign.h"

class NavigationHeader : public CapsLabelAlign
{
		Q_OBJECT

	public:
		explicit NavigationHeader(QWidget *parent = nullptr) : CapsLabelAlign(parent)
		{
		}
};

#endif //NAVIGATIONHEADER_H
