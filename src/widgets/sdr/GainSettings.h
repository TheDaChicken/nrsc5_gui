//
// Created by TheDaChicken on 12/13/2024.
//

#ifndef GAINSETTINGS_H
#define GAINSETTINGS_H

#include <PortSDR.h>
#include <QButtonGroup>
#include <QRadioButton>
#include <QWidget>

#include "widgets/text/TextSlider.h"

class GainSettings : public QGroupBox
{
	Q_OBJECT
	public:
		explicit GainSettings(QWidget *parent = nullptr);
		~GainSettings() override;

		void UpdateGainSettings();

	    void UpdateGainSliders();
	    void UpdateGainModes();

	    void ClearGainSliders();
		void ClearGainModes();
	private slots:
	    void GainFreelyChanged(std::string_view stage, int value);
		void GainChanged(int value);
	private:
		void CreateGainSlider(const PortSDR::Gain& gain);

		bool freely_gain_mode_ = false;

		QVBoxLayout	*main_layout_;

		QWidget *modes_frame_;
		QHBoxLayout *modes_layout_;
		QRadioButton *mode_free_gain_;

		QButtonGroup *modes_button;
		QList<TextSlider*> gain_sliders_;
};

#endif //GAINSETTINGS_H
