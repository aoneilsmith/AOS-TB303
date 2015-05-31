#include "Oscillator.h"

COscillator::COscillator(void)
{
	m_dSampleRate = 44100;	

	m_dModulo = 0.0;		
	m_dInc = 0.0;			

	m_dModulo2 = 0.0;
	m_dInc2 = 0.0;			

	m_dFo = 440.0;			
	m_dFo2 = 440.0;		
	m_dOscFo = 440.0; // GUI
	m_dDetuneCents = 5.0;

	m_bSquareEdgeRising = false;
	m_bSquareEdgeRising2 = false;

	m_dPulseWidth = 50.0;	
	m_dPulseWidthControl = 50.0; // GUI

	srand(time(NULL));
	m_uPNRegister = rand();
	m_nRSHCounter = -1; // flag for reset condition
	m_dRSHValue = 0.0;

	m_dAmplitude = 1.0;
	m_dHardSyncRatio = 1.0;			

	m_dFoModLin = 0.0;
	m_dFoModExp = 0.0;
	m_dFoLowLimit = 20.0;
	m_dFoHighLimit = 20480;
	m_dFoEGMod = 0.0;
	m_dEGModRange = 12;		// semitones
	m_dFoModIntensity = 0.0; // note, defaults to full intensity!
	m_dEGModIntensity = 0.0;
	m_dFoModExpRange = 12;	// semitones
	m_dPitchBendMod = 0.0;
	m_dPitchBendRange = 12; // semitones

	m_nOctave = 0.0;
	m_nSemitones = 0.0;
	m_nCents = 0.0;
	m_dPWMod = 0.0;

	m_bNoteOn = false;
	m_bUnisonMode = false;
}

COscillator::~COscillator(void)
{
}
	
// virtual functions; base class implementations
void COscillator::reset()
{
	// Pitched modulos - wavetables start at 0.0
	m_dModulo = 0.0;	
	m_dModulo2 = 0.0;	
		
	// needed fror triangle algorithm, DPW
	m_dDPWSquareModulator = -1.0;
	m_dDPWSquareModulator2 = -1.0;

	// flush DPW registers
	m_dDPWz1_1 = 0.0;
	m_dDPWz1_2 = 0.0;

	srand(time(NULL));
	m_uPNRegister = rand();
	m_nRSHCounter = -1; // flag for reset condition
	m_dRSHValue = 0.0;

	m_bSquareEdgeRising = false;
	m_bSquareEdgeRising2 = false;
	m_bNoteOn = false;

	m_dPWMod = 0.0;
	m_dPitchBendMod = 0.0;
}

// recalculate due to change in fo
void COscillator::updateOscillator()
{
	// ignore LFO mode for noise sources
	if(m_uWaveform == rsh || m_uWaveform == qrsh)
	   m_uLFOMode = free;
	
	// --- Frequency Modulation --- //
	// 
	// calculate pitch shifts for EACH modulation source
	double dExpMod = pitchShiftMultiplier(m_dFoModIntensity*m_dFoModExp*m_dFoModExpRange);
	double dEGMod = pitchShiftMultiplier(m_dEGModIntensity*m_dFoEGMod*m_dEGModRange);
	double dPitchBendMod = pitchShiftMultiplier(m_dPitchBendMod*m_dPitchBendRange);
	double dOctaveMod = pitchShiftMultiplier((double)m_nOctave*12.0);
	double dSemiMod = pitchShiftMultiplier((double)m_nSemitones);
	double dCentsMod = pitchShiftMultiplier((double)m_nCents/100.0); // cent is 1/100th of a semitone
	
	// combine all together
	// m_dOscFo is the MIDI pitch for pitched oscillators, or fo for LFO
	m_dFo = m_dOscFo*dExpMod*dEGMod*dPitchBendMod*dOctaveMod*dSemiMod*dCentsMod;
	
	// +/- 20480 for FM
	if(m_dFo > m_dFoHighLimit)
		m_dFo = m_dFoHighLimit;
	if(m_dFo < -m_dFoHighLimit)
		m_dFo = -m_dFoHighLimit;

	// calculate increment
	m_dInc = m_dFo/m_dSampleRate;

	// setup the unison mode clone
	if(m_bUnisonMode && m_uWaveform != HSSAW)
		m_dFo2 = pitchShiftMultiplier(m_dDetuneCents/100.0)*m_dFo;
	else
		m_dFo2 = m_dHardSyncRatio*m_dFo;

	// inc for unison mode
	m_dInc2 = m_dFo2/m_dSampleRate;

	// --- Pulse Width Modulation --- //
	// limits are 1% and 99%
	m_dPulseWidth = m_dPulseWidthControl + m_dPWMod*(98.0 - 2.0)/2.0; 

	m_dPulseWidth = min(m_dPulseWidth, 98.0);
	m_dPulseWidth = max(m_dPulseWidth, 2.0);
}
