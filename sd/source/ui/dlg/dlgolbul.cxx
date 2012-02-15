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
#include "precompiled_sd.hxx"

#ifdef SD_DLLIMPLEMENTATION
#undef SD_DLLIMPLEMENTATION
#endif

#include "OutlineBulletDlg.hxx"

#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#include <sfx2/objsh.hxx>
#include <svx/drawitem.hxx>
#include <editeng/bulitem.hxx>
#include <editeng/eeitem.hxx>

#include <editeng/numitem.hxx>

#include <svx/dialogs.hrc>
#include <svl/intitem.hxx>
#include <svx/svdmark.hxx>
#include "View.hxx"
#include <svx/svdobj.hxx>
#include <svl/style.hxx>
#include <drawdoc.hxx>

#ifndef _SD_SDRESID_HXX
#include "sdresid.hxx"
#endif

#include "glob.hrc"
#include "dlgolbul.hrc"
#include "bulmaper.hxx"
#include "DrawDocShell.hxx"
#include <svx/svxids.hrc>
#include <svl/aeitem.hxx>

namespace sd {

/*************************************************************************
|*
|* Konstruktor des Tab-Dialogs: Fuegt die Seiten zum Dialog hinzu
|*
\************************************************************************/

OutlineBulletDlg::OutlineBulletDlg(
    ::Window* pParent,
    const SfxItemSet* pAttr,
    ::sd::View* pView )
    : SfxTabDialog	( pParent, SdResId(TAB_OUTLINEBULLET) ),
      aInputSet		( *pAttr ),
      bTitle			( sal_False ),
      pSdView			( pView )
{
	FreeResource();

	aInputSet.MergeRange( SID_PARAM_NUM_PRESET, SID_PARAM_CUR_NUM_LEVEL );
	aInputSet.Put( *pAttr );

	pOutputSet = new SfxItemSet( *pAttr );
	pOutputSet->ClearItem();

	sal_Bool bOutliner = sal_False;

	// Sonderbehandlung wenn eine Title Objekt selektiert wurde
	if( pView )
	{
		const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
		const sal_uLong nCount = rMarkList.GetMarkCount();
		for(sal_uLong nNum = 0; nNum < nCount; nNum++)
		{
			SdrObject* pObj = rMarkList.GetMark(nNum)->GetMarkedSdrObj();
			if( pObj->GetObjInventor() == SdrInventor )
			{

				switch(pObj->GetObjIdentifier())
				{
				case OBJ_TITLETEXT:
					bTitle = sal_True;
					break;
				case OBJ_OUTLINETEXT:
					bOutliner = sal_True;
					break;
				}
			}
		}
	}

	if( SFX_ITEM_SET != aInputSet.GetItemState(EE_PARA_NUMBULLET))
	{
		const SvxNumBulletItem *pItem = NULL;
		if(bOutliner)
		{
			SfxStyleSheetBasePool* pSSPool = pView->GetDocSh()->GetStyleSheetPool();
			String aStyleName((SdResId(STR_LAYOUT_OUTLINE)));
			aStyleName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " 1" ) );
			SfxStyleSheetBase* pFirstStyleSheet = pSSPool->Find( aStyleName, SD_STYLE_FAMILY_PSEUDO);
			if( pFirstStyleSheet )
				pFirstStyleSheet->GetItemSet().GetItemState(EE_PARA_NUMBULLET, sal_False, (const SfxPoolItem**)&pItem);
		}

		if( pItem == NULL )
			pItem = (SvxNumBulletItem*) aInputSet.GetPool()->GetSecondaryPool()->GetPoolDefaultItem(EE_PARA_NUMBULLET);

		DBG_ASSERT( pItem, "Kein EE_PARA_NUMBULLET im Pool! [CL]" );

		aInputSet.Put(*pItem, EE_PARA_NUMBULLET);
	}

	/* debug
	if( SFX_ITEM_SET == aInputSet.GetItemState(EE_PARA_NUMBULLET, sal_False, &pItem ))
	{
		SvxNumRule& rItem = *((SvxNumBulletItem*)pItem)->GetNumRule();
		for( int i = 0; i < 9; i++ )
		{
			SvxNumberFormat aNumberFormat = rItem.GetLevel(i);
		}
	}
	*/

	if(bTitle && aInputSet.GetItemState(EE_PARA_NUMBULLET,sal_True) == SFX_ITEM_ON )
	{
		SvxNumBulletItem* pItem = (SvxNumBulletItem*)aInputSet.GetItem(EE_PARA_NUMBULLET,sal_True);
		SvxNumRule* pRule = pItem->GetNumRule();
		if(pRule)
		{
			SvxNumRule aNewRule( *pRule );
			aNewRule.SetFeatureFlag( NUM_NO_NUMBERS, sal_True );

			SvxNumBulletItem aNewItem( aNewRule, EE_PARA_NUMBULLET );
			aInputSet.Put(aNewItem);
		}
	}

	SetInputSet( &aInputSet );

	if(!bTitle)
		AddTabPage(RID_SVXPAGE_PICK_SINGLE_NUM);
	else
		RemoveTabPage( RID_SVXPAGE_PICK_SINGLE_NUM );

	AddTabPage( RID_SVXPAGE_PICK_BULLET  );
	AddTabPage( RID_SVXPAGE_PICK_BMP   );
	AddTabPage(RID_SVXPAGE_NUM_OPTIONS 	);
	AddTabPage(RID_SVXPAGE_NUM_POSITION	);

}

OutlineBulletDlg::~OutlineBulletDlg()
{
	delete pOutputSet;
}

void OutlineBulletDlg::PageCreated( sal_uInt16 nId, SfxTabPage &rPage )
{
	switch ( nId )
	{
		case RID_SVXPAGE_NUM_OPTIONS:
		{
			if( pSdView )
			{
				FieldUnit eMetric = pSdView->GetDoc()->GetUIUnit();
				SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));
				aSet.Put ( SfxAllEnumItem(SID_METRIC_ITEM,(sal_uInt16)eMetric));
				rPage.PageCreated(aSet);
			}
		}
		break;
		case RID_SVXPAGE_NUM_POSITION:
		{
			if( pSdView )
			{
				FieldUnit eMetric = pSdView->GetDoc()->GetUIUnit();
				SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));
				aSet.Put ( SfxAllEnumItem(SID_METRIC_ITEM,(sal_uInt16)eMetric));
				rPage.PageCreated(aSet);
			}
		}
		break;
	}
}

const SfxItemSet* OutlineBulletDlg::GetOutputItemSet() const
{
	SfxItemSet aSet( *SfxTabDialog::GetOutputItemSet() );
	pOutputSet->Put( aSet );

	const SfxPoolItem *pItem = NULL;
	if( SFX_ITEM_SET == pOutputSet->GetItemState(pOutputSet->GetPool()->GetWhich(SID_ATTR_NUMBERING_RULE), sal_False, &pItem ))
	{
		SdBulletMapper::MapFontsInNumRule( *((SvxNumBulletItem*)pItem)->GetNumRule(), *pOutputSet );

/* #i35937#
		SfxUInt16Item aBulletState( EE_PARA_BULLETSTATE, 1 );
		pOutputSet->Put(aBulletState);
*/
	}

/* #i35937#
	SdBulletMapper::PostMapNumBulletForDialog( *pOutputSet );
*/

	if(bTitle && pOutputSet->GetItemState(EE_PARA_NUMBULLET,sal_True) == SFX_ITEM_ON )
	{
		SvxNumBulletItem* pBulletItem = (SvxNumBulletItem*)pOutputSet->GetItem(EE_PARA_NUMBULLET,sal_True);
		SvxNumRule* pRule = pBulletItem->GetNumRule();
		if(pRule)
			pRule->SetFeatureFlag( NUM_NO_NUMBERS, sal_False );
	}

	return pOutputSet;
}

} // end of namespace sd
