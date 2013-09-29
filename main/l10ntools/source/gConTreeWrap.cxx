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
#include "gConTree.hxx"



/*****************************************************************************
 ********************   G C O N T R E E W R A P . C X X   ********************
 *****************************************************************************
 * This includes the c code generated by flex
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_tree::convert_tree(l10nMem& crMemory)
                          : convert_gen_impl(crMemory),
                            mcOutputFiles(NULL),
                            meStateTag(STATE_TAG_NONE),
                            meStateVal(STATE_VAL_NONE),
                            miCntLanguages(0)
                            
{
  // tree files are written through a local routine
  mbLoadMode = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_tree::~convert_tree()
{
  if (mcOutputFiles)
  {
    for (int i = 0; i < miCntLanguages; ++i)
      mcOutputFiles[i].close();
    delete[] mcOutputFiles;
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
namespace TreeWrap
{
#define IMPLptr convert_gen_impl::mcImpl
#define LOCptr ((convert_tree *)convert_gen_impl::mcImpl)
#include "gConTree_yy.c"
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::execute()
{
  std::string sLang;
  std::string sFile, sFile2;

  if (mbMergeMode)
    throw l10nMem::showError("Merge not implemented");

  // prepare list with languages
  if (mbMergeMode)
  {
    miCntLanguages = mcMemory.prepareMerge();
    mcOutputFiles  = new std::ofstream[miCntLanguages];

    for (int i = 0; mcMemory.getMergeLang(sLang, sFile); ++i)
    {
      sFile2 = sLang + "/" + msSourceFile;
      sFile  = msTargetPath + sFile2;
      mcOutputFiles[i].open(sFile.c_str(), std::ios::binary); 
      if (!mcOutputFiles[i].is_open())
      {
        if (!convert_gen::createDir(msTargetPath, sFile2))
          throw l10nMem::showError("Cannot create missing directories (" + sFile + ") for writing");

        mcOutputFiles[i].open(sFile.c_str(), std::ios::binary); 
        if (!mcOutputFiles[i].is_open())
          throw l10nMem::showError("Cannot open file (" + sFile + ") for writing");
      }
    }
  }

  // run analyzer
  TreeWrap::yylex();

  // dump last line
  copySourceSpecial(NULL,3);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::setString(char *yytext)
{
  switch (meStateVal)
  {
    case STATE_VAL_NONE:
         copySourceSpecial(yytext, 0);
         break;

    case STATE_VAL_APPL:
         msAppl = copySourceSpecial(yytext, 0);
         break;

    case STATE_VAL_ID:
         msId = copySourceSpecial(yytext, 0);
         msId.erase(msId.size()-1);
         break;

    case STATE_VAL_TITLE:
         std::string sText = copySourceSpecial(yytext, 1);
         sText.erase(sText.size()-1);
         mcMemory.setSourceKey(miLineNo, msSourceFile, msId, sText);
         break;
  }
  meStateVal = STATE_VAL_NONE;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::setState(char *yytext, STATE_TAG eNewStateTag, STATE_VAL eNewStateVAL)
{
  copySourceSpecial(yytext, 0);
  msCollector.clear();
  meStateTag = eNewStateTag;
  meStateVal = eNewStateVAL;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::setValue(char *yytext)
{
  mcMemory.setSourceKey(miLineNo, msSourceFile, msId, msCollector);
  copySourceSpecial(yytext, 2);

  meStateTag = STATE_TAG_NONE;
  meStateVal = STATE_VAL_NONE;
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::string& convert_tree::copySourceSpecial(char *yytext, int iType)
{
  std::string& sText = copySource(yytext, false);
  std::string  sLang, sTemp;
  int          i;

  // Handling depends on iType
  switch (iType)
  {
    case 0: // Used for tokens that are to be copied 1-1, 
            if (mbMergeMode)
            {
              msLine += yytext;
              if (*yytext == '\n')
              {
                for (i = 0; i < miCntLanguages; ++i)
                  writeSourceFile(msLine, i);
                msLine.clear();
              }
            }
            break;

    case 1: // Used for title token, are to replaced with languages
            if (mbMergeMode)
            {
              mcMemory.prepareMerge();
              for (i = 0; i < miCntLanguages; ++i)
              {
                writeSourceFile(msLine, i);
                mcMemory.getMergeLang(sLang, sTemp);
                writeSourceFile(sTemp,i);
              }
              msLine.clear();
            }
            break;

    case 2: // Used for token at end of value, language text are to be inserted and then token written
            if (mbMergeMode)
            {
              mcMemory.prepareMerge();
              for (i = 0; i < miCntLanguages; ++i)
              {
                writeSourceFile(msLine, i);
                mcMemory.getMergeLang(sLang, sTemp);
                writeSourceFile(sTemp,i);
                std::string sYY(yytext);
                writeSourceFile(sYY, i);
              }
              msLine.clear();
            }
            break;

    case 3: // Used for EOF 
            if (mbMergeMode)
            {
              for (i = 0; i < miCntLanguages; ++i)
                writeSourceFile(msLine, i);
            }
            break;
  }
  return sText;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::writeSourceFile(std::string& sText, int inx)
{
  if (sText.size() && mcOutputFiles[inx].is_open())
    mcOutputFiles[inx].write(sText.c_str(), sText.size());
}
