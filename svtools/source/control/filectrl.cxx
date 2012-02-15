/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#define _SV_FIELCTRL_CXX
#include <tools/urlobj.hxx>
#include <svtools/svtdata.hxx>
#include <svtools/filectrl.hxx>
#include <filectrl.hrc>

// =======================================================================

FileControl::FileControl( Window* pParent, WinBits nStyle, FileControlMode nFlags ) :
	Window( pParent, nStyle|WB_DIALOGCONTROL ),
	maEdit( this, (nStyle&(~WB_BORDER))|WB_NOTABSTOP ),
	maButton( this, (nStyle&(~WB_BORDER))|WB_NOLIGHTBORDER|WB_NOPOINTERFOCUS|WB_NOTABSTOP ),
	maButtonText( SvtResId( STR_FILECTRL_BUTTONTEXT ) ),
	mnFlags( nFlags ),
	mnInternalFlags( FILECTRL_ORIGINALBUTTONTEXT )
{
	maButton.SetClickHdl( LINK( this, FileControl, ButtonHdl ) );
	mbOpenDlg = sal_True;

	maButton.Show();
	maEdit.Show();

	SetCompoundControl( sal_True );

	SetStyle( ImplInitStyle( GetStyle() ) );
}

// -----------------------------------------------------------------------

WinBits FileControl::ImplInitStyle( WinBits nStyle )
{
	if ( !( nStyle & WB_NOTABSTOP ) )
	{
		maEdit.SetStyle( (maEdit.GetStyle()|WB_TABSTOP)&(~WB_NOTABSTOP) );
		maButton.SetStyle( (maButton.GetStyle()|WB_TABSTOP)&(~WB_NOTABSTOP) );
	}
	else
	{
		maEdit.SetStyle( (maEdit.GetStyle()|WB_NOTABSTOP)&(~WB_TABSTOP) );
		maButton.SetStyle( (maButton.GetStyle()|WB_NOTABSTOP)&(~WB_TABSTOP) );
	}

    const WinBits nAlignmentStyle = ( WB_TOP | WB_VCENTER | WB_BOTTOM );
    maEdit.SetStyle( ( maEdit.GetStyle() & ~nAlignmentStyle ) | ( nStyle & nAlignmentStyle ) );

	if ( !(nStyle & WB_NOGROUP) )
		nStyle |= WB_GROUP;

	if ( !(nStyle & WB_NOBORDER ) )
		nStyle |= WB_BORDER;

	nStyle &= ~WB_TABSTOP;

	return nStyle;
}

// -----------------------------------------------------------------------

FileControl::~FileControl()
{
}

// -----------------------------------------------------------------------

void FileControl::SetText( const XubString& rStr )
{
	maEdit.SetText( rStr );
	if ( mnFlags & FILECTRL_RESIZEBUTTONBYPATHLEN )
		Resize();
}

// -----------------------------------------------------------------------

XubString FileControl::GetText() const
{
	return maEdit.GetText();
}

// -----------------------------------------------------------------------

void FileControl::StateChanged( StateChangedType nType )
{
	if ( nType == STATE_CHANGE_ENABLE )
	{
		maEdit.Enable( IsEnabled() );
		maButton.Enable( IsEnabled() );
	}
	else if ( nType == STATE_CHANGE_ZOOM )
	{
		GetEdit().SetZoom( GetZoom() );
		GetButton().SetZoom( GetZoom() );
	}
	else if ( nType == STATE_CHANGE_STYLE )
	{
		SetStyle( ImplInitStyle( GetStyle() ) );
	}
	else if ( nType == STATE_CHANGE_CONTROLFONT )
	{
		GetEdit().SetControlFont( GetControlFont() );
		// Fuer den Button nur die Hoehe uebernehmen, weil in
		// HTML immer Courier eingestellt wird.
		Font aFont = GetButton().GetControlFont();
		aFont.SetSize( GetControlFont().GetSize() );
		GetButton().SetControlFont( aFont );
	}
	else if ( nType == STATE_CHANGE_CONTROLFOREGROUND )
	{
		GetEdit().SetControlForeground( GetControlForeground() );
		GetButton().SetControlForeground( GetControlForeground() );
	}
	else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
	{
		GetEdit().SetControlBackground( GetControlBackground() );
		GetButton().SetControlBackground( GetControlBackground() );
	}
	Window::StateChanged( nType );
}

// -----------------------------------------------------------------------

void FileControl::Resize()
{
	static long ButtonBorder = 10;

	if( mnInternalFlags & FILECTRL_INRESIZE )
		return;
	mnInternalFlags |= FILECTRL_INRESIZE;//InResize = sal_True

	Size aOutSz = GetOutputSizePixel();
	long nButtonTextWidth = maButton.GetTextWidth( maButtonText );
	if ( ((mnInternalFlags & FILECTRL_ORIGINALBUTTONTEXT) == 0) ||
		( nButtonTextWidth < aOutSz.Width()/3 &&
		( mnFlags & FILECTRL_RESIZEBUTTONBYPATHLEN
		? ( maEdit.GetTextWidth( maEdit.GetText() )
			<= aOutSz.Width() - nButtonTextWidth - ButtonBorder )
		: sal_True ) )
	   )
	{
		maButton.SetText( maButtonText );
	}
	else
	{
		XubString aSmallText( RTL_CONSTASCII_USTRINGPARAM( "..." ) );
		maButton.SetText( aSmallText );
		nButtonTextWidth = maButton.GetTextWidth( aSmallText );
	}

	long nButtonWidth = nButtonTextWidth+ButtonBorder;
	maEdit.SetPosSizePixel( 0, 0, aOutSz.Width()-nButtonWidth, aOutSz.Height() );
	maButton.SetPosSizePixel( aOutSz.Width()-nButtonWidth, 0, nButtonWidth, aOutSz.Height() );

	mnInternalFlags &= ~FILECTRL_INRESIZE; //InResize = sal_False
}

// -----------------------------------------------------------------------

IMPL_LINK( FileControl, ButtonHdl, PushButton*, EMPTYARG )
{
	ImplBrowseFile( );

	return 0;
}

// -----------------------------------------------------------------------

void FileControl::GetFocus()
{
	maEdit.GrabFocus();
}

// -----------------------------------------------------------------------

void FileControl::Draw( OutputDevice* pDev, const Point& rPos, const Size& rSize, sal_uLong nFlags )
{
	WinBits nOldEditStyle = GetEdit().GetStyle();
	if ( GetStyle() & WB_BORDER )
		GetEdit().SetStyle( nOldEditStyle|WB_BORDER );
	GetEdit().Draw( pDev, rPos, rSize, nFlags );
	if ( GetStyle() & WB_BORDER )
		GetEdit().SetStyle( nOldEditStyle );
}

// -----------------------------------------------------------------------

void FileControl::SetButtonText( const XubString& rStr )
{
	mnInternalFlags &= ~FILECTRL_ORIGINALBUTTONTEXT;
	maButtonText = rStr;
	Resize();
}

// -----------------------------------------------------------------------

void FileControl::ResetButtonText()
{
	mnInternalFlags |= FILECTRL_ORIGINALBUTTONTEXT;
	maButtonText = XubString( SvtResId( STR_FILECTRL_BUTTONTEXT ) );
	Resize();
}


