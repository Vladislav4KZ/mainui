/*
Slider.h - slider
Copyright (C) 2010 Uncle Mike
Copyright (C) 2017 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "BaseMenu.h"
#include "Slider.h"
#include "Utils.h"

CMenuSlider::CMenuSlider() : CMenuEditable()
{
	iColor = uiColorWhite;
	iFocusColor = uiColorWhite;

	size.w = 200;
	size.h = 4;

	m_flRange = 1.0f;

	eFocusAnimation = QM_HIGHLIGHTIFFOCUS;

	SetCharSize( 12, 24 );

	iFlags |= QMF_DROPSHADOW;
}

/*
=================
CMenuSlider::Init
=================
*/
void CMenuSlider::VidInit(  )
{
	if( m_flRange < 0.05f )
		m_flRange = 0.05f;

	m_scChSize = charSize.Scale();
	m_scPos = pos.Scale();
	m_scSize = size.Scale();

	m_scSize.h += uiStatic.sliderWidth * 2;
	m_scPos.y -= uiStatic.sliderWidth;

	// scale the center box
	m_scCenterBox.w = m_scSize.w / 5.0f;
	m_scCenterBox.h = 4;

	m_iNumSteps = (m_flMaxValue - m_flMinValue) / m_flRange + 1;
	m_flDrawStep = (float)(m_scSize.w) / (float)m_iNumSteps;
}

/*
=================
CMenuSlider::Key
=================
*/
const char *CMenuSlider::Key( int key, int down )
{
	int sliderX;

	if( !down )
	{
		if( m_iKeepSlider )
		{
			// tell menu about changes
			SetCvarValue( m_flCurValue );
			_Event( QM_CHANGED );
			m_iKeepSlider = false; // button released
		}
		return uiSoundNull;
	}

	switch( key )
	{
	case K_MOUSE1:
		if( !UI_CursorInRect( m_scPos.x, m_scPos.y - 20, m_scSize.w, m_scSize.h + 40 ) )
		{
			m_iKeepSlider = false;
			return uiSoundNull;
		}

		m_iKeepSlider = true;

		// immediately move slider into specified place
		int	dist, numSteps;

		dist = uiStatic.cursorX - m_scPos.x - (m_scCenterBox.w>>2);
		numSteps = round(dist / m_flDrawStep);
		m_flCurValue = bound( m_flMinValue, numSteps * m_flRange, m_flMaxValue );

		// tell menu about changes
		SetCvarValue( m_flCurValue );
		_Event( QM_CHANGED );

		return uiSoundNull;
		break;
	case K_LEFTARROW:
		m_flCurValue -= m_flRange;

		if( m_flCurValue < m_flMinValue )
		{
			m_flCurValue = m_flMinValue;
			return uiSoundBuzz;
		}

		// tell menu about changes
		SetCvarValue( m_flCurValue );
		_Event( QM_CHANGED );

		return uiSoundKey;
		break;
	case K_RIGHTARROW:
		m_flCurValue += m_flRange;

		if( m_flCurValue > m_flMaxValue )
		{
			m_flCurValue = m_flMaxValue;
			return uiSoundBuzz;
		}

		// tell menu about changes
		SetCvarValue( m_flCurValue );
		_Event( QM_CHANGED );

		return uiSoundKey;
		break;
	}

	return 0;
}

/*
=================
CMenuSlider::Draw
=================
*/
void CMenuSlider::Draw( void )
{
	bool	shadow;
	int	textHeight, sliderX;


	shadow = (iFlags & QMF_DROPSHADOW);

	if( m_iKeepSlider )
	{
		if( !UI_CursorInRect( m_scPos.x, m_scPos.y - 40, m_scSize.w, m_scSize.h + 80 ) )
			m_iKeepSlider = false;
		else
		{
			int	dist, numSteps;

			// move slider follow the holded mouse button
			dist = uiStatic.cursorX - m_scPos.x - (m_scCenterBox.w>>2);
			numSteps = round(dist / m_flDrawStep);
			m_flCurValue = bound( m_flMinValue, numSteps * m_flRange, m_flMaxValue );

			// tell menu about changes
			SetCvarValue( m_flCurValue );
			_Event( QM_CHANGED );
		}
	}

	// keep value in range
	m_flCurValue = bound( m_flMinValue, m_flCurValue, m_flMaxValue );

	// calc slider position
	//sliderX = m_scPos.x + (m_flDrawStep * (m_flCurValue / m_flRange)); // TODO: fix behaviour when values goes negative
	//sliderX = bound( m_scPos.x, sliderX, m_scPos.x + m_scSize.w - uiStatic.sliderWidth);
	sliderX = m_scPos.x + ( ( m_flCurValue - m_flMinValue ) / ( m_flMaxValue - m_flMinValue ) ) * ( m_scSize.w - (m_scCenterBox.w) + (m_scCenterBox.w>>2) );


	UI_DrawRectangleExt( m_scPos.x, m_scPos.y + uiStatic.sliderWidth, m_scSize.w, m_scCenterBox.h, uiInputBgColor, uiStatic.sliderWidth );
	if( eFocusAnimation == QM_HIGHLIGHTIFFOCUS && this == m_pParent->ItemAtCursor())
	{
		UI_DrawPic( sliderX, m_scPos.y, m_scCenterBox.w, m_scSize.h, uiColorHelp, UI_SLIDER_MAIN );
	}
	else
		UI_DrawPic( sliderX, m_scPos.y, m_scCenterBox.w, m_scSize.h, uiColorWhite, UI_SLIDER_MAIN );


	textHeight = m_scPos.y - (m_scChSize.h * 1.5f);
	UI_DrawString( m_scPos.x, textHeight, m_scSize.w, m_scChSize.h, szName, uiColorHelp, true, m_scChSize.w, m_scChSize.h, eTextAlignment, shadow );
}

void CMenuSlider::UpdateEditable()
{
	float flValue = EngFuncs::GetCvarFloat( m_szCvarName );

	m_flCurValue = flValue;
}