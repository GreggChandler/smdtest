/*
 *	Copyright (C) 2017-2017, by Enthusiastic Software
 *	All Rights Reserved
 *
 *	smdtest.cpp
 *
 */

#define	ESXONECORE	0	/* Non-Zero for Single Core		*/

#include	<csignal>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string>
#include	<string.h>
#include	<unistd.h>

#include	"esxtypes.h"

#include	"esxehost.h"

#include	"esxxm.h"

#define		DBG()	printf("%s:%d\n",__FILE__,__LINE__)

class SMDTest : public ESXAppHost {

  public:
    SMDTest()
    : ESXAppHost() {
#if	ESXONECORE
      uSizeRows = 1;
      uSizeCols = 1;
#endif
      }

    bool ProcessArgs(LPCTSTR pArg,int & iArgCount,LPCTSTR * & pArgValues);
    void Exec();
    void Yield();

    LPCTSTR App(uint uThread);

  };

bool SMDTest::ProcessArgs(
  LPCTSTR    	pArg,
  int &		iArgCount,
  LPCTSTR * & 	pArgValues)
  {
  return(false);
  }

void SMDTest::Exec()
  {
  FNA("SMDTest::Exec()");
  //
  double	dElapsed;
  time_t	tStart;
  time_t	tStop;
  int		iHours;
  int		iMinutes;
  double	dSeconds;
  //
  time(&tStart);
  //
  while(!epilogPoll())
    Yield();
  //
  time(&tStop);
  dElapsed = difftime(tStop,tStart);
  iHours = (int)(dElapsed / (60*60));
  dElapsed -= iHours * 60 * 60;
  iMinutes = (int)(dElapsed / 60);
  dElapsed -= iMinutes * 60;
#if	0==0
  epiPrintF("Elapsed: %02d:%02d:%06.3f\n",
    iHours,iMinutes,dElapsed);
#endif
  bDumpMemory = false;
  }

void SMDTest::Yield()
  {
  PollSignal();
  epiConfigHost.CurrentWorkgroup()->CheckStatus(180);
  }

LPCTSTR SMDTest::App(uint uThread)
  {
  switch(uThread) {
#if	0==1
    case 0:
      return("e-smdtest0.elf");
#endif
    default:
      return("e-smdtest.elf");
    };
  }

SMDTest		AppHost;
ESXAppHost *	ESXAppHost::pAppHost = &AppHost;
