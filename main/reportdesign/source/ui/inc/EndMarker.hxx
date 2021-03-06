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


#ifndef RPTUI_ENDMARKER_HXX
#define RPTUI_ENDMARKER_HXX

#include "ColorListener.hxx"

namespace rptui
{
    /** \class OEndMarker
     *  \brief Defines the right side of a graphical section.
     */
	class OEndMarker : public OColorListener
	{
        OEndMarker(OEndMarker&);
        void operator =(OEndMarker&);
    protected:
		virtual void ImplInitSettings();
	public:
		OEndMarker(Window* _pParent,const ::rtl::OUString& _sColorEntry);
		virtual ~OEndMarker();

		// windows
		virtual void    Paint( const Rectangle& rRect );
        virtual void	MouseButtonDown( const MouseEvent& rMEvt );
	};
}
#endif // RPTUI_ENDMARKER_HXX

