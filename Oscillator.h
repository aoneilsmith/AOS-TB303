#pragma once
#include "pluginconstants.h"
#include "synthfunctions.h"

class COscillator
{
public:
	COscillator(void);
	virtual ~COscillator(void);

protected:
	double m_dSampleRate;	// fs
	double m_dModulo;		// modulo counter 0->1
	double m_dInc;			// phase inc = fo/fs
	double m_dModulo2;		// modulo counter 0->1 for unison mode or hard sync 
	double m_dInc2;			// phase inc = fo/fs   for unison mode or hard sync 
	
	double m_dOscFo;		// oscillator frequency from MIDI note number
	double m_dHardSyncRatio;// for hard sync waveform
	
	double m_dFo;			// current value for calculation
	double m_dFo2;			// current value for calculation, unison/hard sync

	int m_nOctave;			// octave tweak
	int m_nSemitones;		// semitones tweak
	int m_nCents;			// cents tweak

	double m_dPulseWidthControl;	// pulse width in % (sqr only) from GUI
	double m_dPulseWidth;			// pulse width in % for calculation
	
	bool   m_bSquareEdgeRising; // hysteresis for square edge
	bool   m_bSquareEdgeRising2; // hysteresis for square edge
	
	UINT   m_uPNRegister;	// for PN Noise sequence
	int    m_nRSHCounter;	// random sample/hold counter
	double m_dRSHValue;		// currnet rsh output
	
	// for DPW
	double m_dDPWSquareModulator;	// square toggle for DPW
	double m_dDPWSquareModulator2;	// square toggle for DPW
	
	// memory registers for differentiators for DPW
	double m_dDPWz1_1;
	double m_dDPWz1_2;
	
	// for modulation of oscillator pitch
	double m_dFoModExp;		 /* modulation input -1 to +1 */
	double m_dFoModExpRange; /* in SEMITONES */
	double m_dFoModIntensity;/* control the pitch mod intensity -1 to +1 */

	double m_dFoEGMod;		 /* modulation input for EG 0 to +1 */
	double m_dEGModRange;	 /* in SEMITONES */
	double m_dEGModIntensity;/*control the EG intensity -1 to +1 */

	double m_dPitchBendMod;	  /* modulation input -1 to +1 */
	double m_dPitchBendRange; /* in SEMITONES */

	double m_dFoModLin;		 /* FM Synth modulation input -1 to +1 */

	double m_dFoLowLimit;	 /* low limit on fc */
	double m_dFoHighLimit;	 /* high limit on fc */

	// for PWM
	double m_dPWMod;		/* modulation input for PWM -1 to +1 */

	// amplitude raw 0->1 
	double m_dAmplitude;

	// for LFOs
	enum {sine,usaw,dsaw,tri,square,expo,rsh,qrsh};

	// for PITCHED Oscillators
	enum {SINE,SAW1,SAW2,SAW3,HSSAW,TRI,SQUARE,NOISE,PNOISE,SAW303,SQR303};
	UINT m_uWaveform;	// to store type
	
	// for LFOs - MODE
	enum {sync,shot,free};
	UINT m_uLFOMode;	// to store MODE

	// oscillator run flag
	bool m_bNoteOn;

	// provide a second detuned output
	bool m_bUnisonMode;
	double m_dDetuneCents;

	// true if modulo wrapped
	inline bool checkWrapModulo()
	{	
		// for positive frequencies
		if(m_dInc > 0 && m_dModulo >= 1.0) 
		{
			m_dModulo -= 1.0; 
			return true;
		}

		// for negative frequencies
		if(m_dInc < 0 && m_dModulo <= 0.0) 
		{
			m_dModulo += 1.0; 
			return true;
		}
		return false;
	}
	
	inline bool checkWrapModulo2()
	{	
		// for positive frequencies
		if(m_dInc > 0 && m_dModulo2 >= 1.0) 
		{
			m_dModulo2 -= 1.0; 
			return true;
		}

		// for negative frequencies
		if(m_dInc < 0 && m_dModulo2 <= -1.0) 
		{
			m_dModulo2 += 1.0; 
			return true;
		}
		return false;
	}
	
	// increment the modulo counters
	inline void incModulo(){m_dModulo += m_dInc;}
	inline void incModulo2(){m_dModulo2 += m_dInc2;}

public:
	// mutators
	inline void setOscFo(double dFo){m_dOscFo = dFo;}
	inline void setHardSyncRatio(double dRatio){m_dHardSyncRatio = dRatio;}
	inline void setPulseWidthControl(double dPW){m_dPulseWidthControl = dPW;}
	inline void setWaveform(UINT uWF){m_uWaveform = uWF;}
	inline void setLFOMode(UINT uMode){m_uLFOMode = uMode;}
	inline void setAmplitude(double dAmp){m_dAmplitude = dAmp;}
	inline void setUnisonMode(bool bUni){m_bUnisonMode = bUni;}

	// tweaks
	inline void setOctave(int dVar){m_nOctave = dVar;}
	inline void setSemitones(int dVar){m_nSemitones = dVar;}
	inline void setCents(int dVar){m_nCents = dVar;}

	// modulation, exponential
	inline void setFoModExp(double dMod){m_dFoModExp = dMod;}
	inline void setFoModIntensity(double dMod){m_dFoModIntensity = dMod;}
	inline void setFoEgMod(double dMod){m_dFoEGMod = dMod;}
	inline void setFoEgModIntensity(double dMod){m_dEGModIntensity = dMod;}
	inline void setPitchBendMod(double dMod){m_dPitchBendMod = dMod;}
	inline void setPitchBendModRange(double dMod){m_dPitchBendRange = dMod;}
	
	// for FM synthesis only
	inline void setFoModLin(double dMod){m_dFoModLin = dMod;}

	// for square waves only
	inline void setPWMod(double dMod){m_dPWMod = dMod;}

	// virtual overrides
	virtual void setSampleRate(double dFs){m_dSampleRate = dFs;}
	
	// reset counters, etc...
	virtual void reset();

	// recalculate due to change in fo
	virtual void updateOscillator();

	// start/stop control
	virtual void startOscillator() = 0;
	virtual void stopOscillator() = 0;

	// render a sample
	virtual double doOscillate() = 0; // pure abstract 


};
