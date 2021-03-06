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



package org.openoffice.xmerge.converter.xml.sxc.pexcel;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;

import org.openoffice.xmerge.util.Debug;
import org.openoffice.xmerge.ConvertData;
import org.openoffice.xmerge.converter.xml.sxc.SpreadsheetDecoder;
import org.openoffice.xmerge.converter.xml.sxc.SxcDocumentDeserializer;
import org.openoffice.xmerge.converter.xml.sxc.pexcel.PocketExcelDecoder;
import org.openoffice.xmerge.converter.xml.sxc.pexcel.records.Workbook;


/**
 *  <p>Pocket Excel implementation of <code>DocumentDeserializer</code>
 *  for the {@link
 *  org.openoffice.xmerge.converter.xml.sxc.pexcel.PluginFactoryImpl
 *  PluginFactoryImpl}.</p>
 *
 *  <p>This converts a set of files in Pocket Excel PXL format to a StarOffice DOM.</p>
 *
 *  @author  Mark Murnane
 */
public final class SxcDocumentDeserializerImpl extends SxcDocumentDeserializer {

    /**
     *  Creates new <code>SxcDocumentDeserializerImpl</code>.
     *
     *  @param  cd  <code>ConvertData</code>  Input data to convert.
     */
    public SxcDocumentDeserializerImpl(ConvertData cd) {
        super(cd);
    }


    /**
     *  This method will be implemented by concrete subclasses and will
     *  return an application-specific decoder.
     *
     *  @param  workbook        The WorkBook name.
     *  @param  worksheetNames  An array of WorkSheet names.
     *  @param  password        The password.
     *
     *  @return  An application-specific <code>SpreadsheetDecoder</code>.
     */
    public SpreadsheetDecoder createDecoder(String workbook,
        String[] worksheetNames, String password) throws IOException {

        return new PocketExcelDecoder(workbook, worksheetNames, password);
    }
    

    /**
     *  This method will return the name of the WorkBook from the
     *  <code>ConvertData</code>.  Allows for situations where the
     *  WorkBook name differs from the PDB name.
     *
     *  Implemented in the Deserializer as the Decoder's constructor
     *  requires a name.
     *
     *  @param  cd  The <code>ConvertData</code>.
     *
     *  @return  The name of the WorkBook. 
     */
    protected String getWorkbookName(ConvertData cd) 
        throws IOException {

        Enumeration e = cd.getDocumentEnumeration();
		Workbook wb = (Workbook) e.nextElement();

		String workbookName = wb.getName();
        return workbookName; 
    }
    

    /**
     *  This method will return an array of WorkSheet names from the
     *  <code>ConvertData</code>.
     *
     *  @param  cd  The <code>ConvertData</code>.
     *
     *  @return  The name of the WorkSheet. 
     */
    protected String[] getWorksheetNames(ConvertData cd) 
        throws IOException {

        Enumeration e = cd.getDocumentEnumeration();
		Workbook wb = (Workbook) e.nextElement();
		Vector v = wb.getWorksheetNames();
		e = v.elements();
		String worksheetNames[] = new String[v.size()];
		int i = 0;
		while(e.hasMoreElements()) {
			worksheetNames[i] = (String) e.nextElement();
			Debug.log(Debug.TRACE,"Worksheet Name : " + worksheetNames[i]);
			i++;
		}
		return worksheetNames; 
    }
}

