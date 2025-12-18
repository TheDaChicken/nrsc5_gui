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

		Q_DISABLE_COPY_MOVE(GainSettings)

		void UpdateTunerStream(PortSDR::Stream *stream);

		void ClearGainSliders();
		void ClearGainModes();

	private:
		void GainFreelyChanged(std::string_view stage, int value) const;

		void UpdateGainSliders();
		void UpdateGainModes();

		void CreateGainSlider(const PortSDR::Gain &gain);

		QVBoxLayout *main_layout_;

		QWidget *modes_frame_;
		QHBoxLayout *modes_layout_;

		QButtonGroup *modes_button;
		QList<TextSlider *> gain_sliders_;

		PortSDR::Stream *stream_{nullptr};
};

#endif //GAINSETTINGS_H
