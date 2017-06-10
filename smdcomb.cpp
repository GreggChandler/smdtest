/*
 *	Copyright (C) 2017-2017, by Enthusiastic Software
 *	All Rights Reserved
 *
 *	smdcomb.cpp	- SMD Combine
 *
 */

#include	<errno.h>
#include	<stdio.h>
#include	<string>
#include	<string.h>

#include	"esxtypes.h"

#include	"esxfatal.h"

char *	fns(char * pString);

LPCTSTR		pFolder = NULL;

int main(int iArgCount,char * pArgValues[])
  {
  FNA(TEXT("smdcomb:main()"));
  int		i;
  size_t	n;
  char *	pArg;
  int		iError;
  FILE *	pFile;
  ESXString	sFileName;
  char		cLine[256];
  try {
    iError = 1;
    /*
     *	Process Arguments
     */
    iArgCount--;
    *pArgValues++;
    while(iArgCount-->0)
      {
      pArg = *pArgValues++;
      if     (strcmp(pArg,"-folder")==0 && iArgCount>0)
	    {
	    iArgCount--;
	    pFolder = *pArgValues++;
	    }
      else
	    ESXTHROW(ESXFatalError(FN(),
	      TEXT("Illegal Argument: %s"),
	      pArg));
      };
    /*
     *	Combine Results
     */
    fprintf(stdout,
      "          c:0"
      "            c:1"
      "            c:2"
      "            c:3\n");
    for(i=0;i<16;i++)
      {
      sFileName = TEXT("");
      if(pFolder!=NULL)
	{
	sFileName = pFolder;
	sFileName += TEXT("/");
	};
      snprintf(cLine,NUMELEM(cLine),TEXT("test%02d.txt"),i);
      sFileName += cLine;
      if((pFile=fopen(sFileName.c_str(),TEXT("r")))==NULL)
	ESXTHROW(ESXFatalError(FN(),
	  TEXT("Unable to open file: %s, %d"),
	  sFileName.c_str(),errno));
      //
      if((i%4)==0)
	fprintf(stdout,"r:%d",i/4);
      //
      cLine[0] = NULLC;
      while(strncmp(cLine,"Avg:",4)!=0 && !feof(pFile) && !ferror(pFile))
	fgets(cLine,NUMELEM(cLine),pFile);
      n = strlen(cLine);
      if(n>0 && cLine[n-1]=='\n')
	cLine[n-1] = NULLC;
      fputs("    ",stdout);
      fputs(fns(cLine+4),stdout);
      //
      cLine[0] = NULLC;
      while(strncmp(cLine,"StdDev::",7)!=0 && !feof(pFile) && !ferror(pFile))
	fgets(cLine,NUMELEM(cLine),pFile);
      n = strlen(cLine);
      if(n>0 && cLine[n-1]=='\n')
	cLine[n-1] = NULLC;
      fputs("(",stdout);
      fputs(fns(cLine+7),stdout);
      fputs(")",stdout);
      //
      if((i%4)==3)
	printf("\n");
      //
      if(fclose(pFile)<0)
	ESXTHROW(ESXFatalError(FN(),
	  TEXT("Unable to close file: %s, %d"),
	  sFileName.c_str(),errno));
      };
    iError = 0;
    } catch(ESXFatalError * e) {
      fprintf(stderr,"%s:%s\n",e->Name(),e->Message());
    } catch(std::exception e) {
      fprintf(stderr,"Standard Exception: %s\n",e.what());
    } catch(...) {
      fprintf(stderr,"Uncaught Exception\n");
      };
  return(iError);
  }

char * fns(char * p)
  {
  while(*p==' ')
    p++;
  return(p);
  }
