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



#ifndef _SDGMOITM_HXX
#define _SDGMOITM_HXX

#include <svtools/grfmgr.hxx>
#include <svl/eitem.hxx>
#include <svx/svddef.hxx>
#include "svx/svxdllapi.h"

//------------------
// SdrGrafModeItem -
//------------------

class SVX_DLLPUBLIC SdrGrafModeItem : public SfxEnumItem
{
public:

							TYPEINFO();

							SdrGrafModeItem( GraphicDrawMode eMode = GRAPHICDRAWMODE_STANDARD ) : SfxEnumItem( SDRATTR_GRAFMODE, (sal_uInt16)eMode ) {}
							SdrGrafModeItem( SvStream& rIn ) : SfxEnumItem( SDRATTR_GRAFMODE, rIn ) {}

	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = NULL ) const;
	virtual SfxPoolItem*	Create( SvStream& rIn, sal_uInt16 nVer ) const;
	virtual sal_uInt16			GetValueCount() const;
	GraphicDrawMode			GetValue() const { return (GraphicDrawMode) SfxEnumItem::GetValue(); }

	virtual String			GetValueTextByPos( sal_uInt16 nPos ) const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
												 SfxMapUnit eCoreMetric,
												 SfxMapUnit ePresMetric,
                                                 String& rText, const IntlWrapper * = 0) const;

};

#endif // _SDGMOITM_HXX
