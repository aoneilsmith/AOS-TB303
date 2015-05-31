#include "BassLine.h"

CBassLine::CBassLine(void)
{
}

CBassLine::~CBassLine(void)
{
}

void CBassLine::reset()
{	

	COscillator::reset();


	if(m_uWaveform == SAW303)
	{
		m_dModulo=0.5;
		m_dModulo2 = 0.5;
	}
	//update
	updateOscillator();
	
	// add your code here, like the homework you did
}

void CBassLine::startOscillator()
{
	reset();
	//set flag
	m_bNoteOn = true;
}

void CBassLine::stopOscillator()
{
	//clear flag
	m_bNoteOn = false;
}

double CBassLine::doSawtooth(double dModulo, double dInc)
{
	double dTrivialSaw = 0.0;
	double dOut = 0.0;
	bool bDone = false;

	//if(m_uWaveform == SAW303 )
		dTrivialSaw = unipolarToBipolar(dModulo);
	//parabolicsine approx
	dOut = dTrivialSaw + polyBLEP(dModulo,abs(dInc), 1.0, false, bDone);
	return dOut;
}

double CBassLine::doSquare(double dModulo, double dInc, bool* pRisingEdge)
{
	// pulse width conversion from %
	double dPW = m_dPulseWidth/100.0;
	
	//trivial square
	double dTrivialSquare = dModulo >= dPW ? -1.0 : +1.0;

	//setup polyBlep
	bool bDone = false;

	// loc = 0 for rising edge, or pw for falling edge
	double dEdgeLocation = *pRisingEdge ? 0.0 : dPW;

	//call function
	double dOut = dTrivialSquare + polyBLEP(dModulo,dInc, 1.0, *pRisingEdge, bDone,dEdgeLocation);

	//toggle edge state
	if(bDone)
		*pRisingEdge = !*pRisingEdge;

	/*	correct for pw
	
		25% -> .667 amplitude (0.5/(1-.25)), +.333 DC offset (1-amp)
		50% -> 1.0 amplitde, 0 DC offset
		75% -> .667 amplitude (0.5/0.75), -.333 DC offset (1-amp)
	*/
	double dCorrection = 1.0;
	if(dPW < 0.5)
	{
		dCorrection = 0.5/(1.0 - dPW);
		dOut = dOut*dCorrection + (1.0 - dCorrection);
	}
	else if(dPW>0.5)
	{
		dCorrection = 0.5/dPW;
		dOut = dOut*dCorrection - (1.0 - dCorrection);
	}
	return dOut;
}

double CBassLine::doOscillate()
{
	if(!m_bNoteOn)
		return 0.0;

	double dOut = 0.0;
	double ySQ=0.0;
	double ySW=0.0;
	double SAW303inc = 0.0;
	double SQR303mod = 0.0;
	double SQR303inc = 0.0;
	double sinx = 0.0;
	bool bWrap = checkWrapModulo();

	
	switch(m_uWaveform)
	{
		case SAW303:
		{	/* //Cool Sawtooth 
			if(m_dModulo>0.1)
			{
				SAW303inc =  -0.5*(m_dInc);
				ySW = doSawtooth(m_dModulo, SAW303inc);
			}
			else if(m_dModulo<0.1)
			{
				SAW303inc = m_dModulo - 1;
				ySW = doSawtooth(m_dModulo, SAW303inc);
			}
			//invert for downsaw
			ySW *= -1.0;
			dOut = ySW;
			*/
			if(m_dModulo>.15)
			{
				//SAW303inc = ((0.1)/(0.5))*(m_dInc);
				SAW303inc = .1*(m_dInc);
				ySW = doSawtooth(m_dModulo,SAW303inc);
			}
			else 
			{
				SAW303inc = m_dInc;
				ySW = 0.1*(doSawtooth(m_dModulo, SAW303inc))-0.65 ;
			}
			ySW *= -1.0;
			dOut = ySW;

			break;
		}
		case SQR303:
		{
			/*
			if(m_dModulo > 0.7)
				SQR303inc = m_dInc * sin(sinx=2.0);
			if(m_dModulo > 0.4)
				SQR303inc = m_dInc * 2.0;
			else if(m_dModulo < 0.4)
				SQR303inc = m_dInc;
			
			//ySQ = doSquare(m_dModulo, m_dInc, &m_bSquareEdgeRising);
	
			
			if(m_dModulo>0.7)
				SQR303inc = m_dInc * 0.3;
			ySQ = doSawtooth(m_dModulo, SQR303inc);*/		
			if(m_dModulo<0.8)
			{
				SAW303inc = 0.2*(m_dInc) + 0.5;
				ySW = doSawtooth(m_dModulo, SAW303inc);
			}
			else
			{
				ySW = 1.0;
			}
			//invert for downsaw
			//ySW *= -1.0;
			//dOut = ySW;
			//ySW = doSawtooth(m_dModulo, m_dInc);
			ySW *= -1.0;
			
			dOut = ySW;
			break;
		}
	}
	// add your code here, wrap modulo, calculate, inc modulo, all justlike
	// the homework you did
	incModulo();
	return dOut;
}