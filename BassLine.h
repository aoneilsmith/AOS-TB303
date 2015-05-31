#pragma once
#include "oscillator.h"

class CBassLine : public COscillator
{
public:
	CBassLine(void);
	~CBassLine(void);

	//polyBLEP
	double doSawtooth(double dModulo, double dInc);
	double doSquare(double dModulo, double dInc, bool* pRisingEdge);

	// enum to match the plugin; can save on m_uWaveform
	enum{SAW303,SQR303};

	// to enable polyBLEP in the oscillator
	UINT m_uPolyBLEP;

	// virtual overrides
	virtual void reset();
	virtual void startOscillator();
	virtual void stopOscillator();
	virtual double doOscillate();
};
